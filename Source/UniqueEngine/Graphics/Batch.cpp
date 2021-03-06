#include "UniquePCH.h"
#include "Batch.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Light.h"
#include "../Graphics/Geometry.h"
#include "../Graphics/Texture.h"
#include "Renderer.h"
#include "Scene/Node.h"
#include "../Graphics/PipelineState.h"

using namespace Diligent;

namespace Unique
{


	inline bool CompareBatchesState(Batch* lhs, Batch* rhs)
	{
		if (lhs->renderOrder_ != rhs->renderOrder_)
			return lhs->renderOrder_ < rhs->renderOrder_;
		else if (lhs->sortKey_ != rhs->sortKey_)
			return lhs->sortKey_ < rhs->sortKey_;
		else
			return lhs->distance_ < rhs->distance_;
	}

	inline bool CompareBatchesFrontToBack(Batch* lhs, Batch* rhs)
	{
		if (lhs->renderOrder_ != rhs->renderOrder_)
			return lhs->renderOrder_ < rhs->renderOrder_;
		else if (lhs->distance_ != rhs->distance_)
			return lhs->distance_ < rhs->distance_;
		else
			return lhs->sortKey_ < rhs->sortKey_;
	}

	inline bool CompareBatchesBackToFront(Batch* lhs, Batch* rhs)
	{
		if (lhs->renderOrder_ != rhs->renderOrder_)
			return lhs->renderOrder_ < rhs->renderOrder_;
		else if (lhs->distance_ != rhs->distance_)
			return lhs->distance_ > rhs->distance_;
		else
			return lhs->sortKey_ < rhs->sortKey_;
	}

	inline bool CompareInstancesFrontToBack(const InstanceData& lhs, const InstanceData& rhs)
	{
		return lhs.distance_ < rhs.distance_;
	}

	inline bool CompareBatchGroupOrder(BatchGroup* lhs, BatchGroup* rhs)
	{
		return lhs->renderOrder_ < rhs->renderOrder_;
	}

	void CalculateShadowMatrix(Matrix4& dest, LightBatchQueue* queue, unsigned split, Renderer* renderer)
	{
		Camera* shadowCamera = queue->shadowSplits_[split].shadowCamera_;
		const IntRect& viewport = queue->shadowSplits_[split].shadowViewport_;

		Matrix3x4 shadowView(shadowCamera->GetView());
		Matrix4 shadowProj(shadowCamera->GetGPUProjection());
		Matrix4 texAdjust(Matrix4::IDENTITY);

		Texture* shadowMap = queue->shadowMap_;
		if (!shadowMap)
			return;

		float width = (float)shadowMap->GetWidth();
		float height = (float)shadowMap->GetHeight();

		Vector3 offset(
			(float)viewport.left_ / width,
			(float)viewport.top_ / height,
			0.0f
		);

		Vector3 scale(
			0.5f * (float)viewport.Width() / width,
			0.5f * (float)viewport.Height() / height,
			1.0f
		);

		// Add pixel-perfect offset if needed by the graphics API
		//const Vector2& pixelUVOffset = Graphics::GetPixelUVOffset();
		//offset.x_ += scale.x_ + pixelUVOffset.x_ / width;
		//offset.y_ += scale.y_ + pixelUVOffset.y_ / height;

#ifdef UNIQUE_OPENGL
		offset.z_ = 0.5f;
		scale.z_ = 0.5f;
		offset.y_ = 1.0f - offset.y_;
#else
		scale.y_ = -scale.y_;
#endif

		// If using 4 shadow samples, offset the position diagonally by half pixel
		//if (renderer->GetShadowQuality() == SHADOWQUALITY_PCF_16BIT || renderer->GetShadowQuality() == SHADOWQUALITY_PCF_24BIT)
		{
			offset.x_ -= 0.5f / width;
			offset.y_ -= 0.5f / height;
		}

		texAdjust.SetTranslation(offset);
		texAdjust.SetScale(scale);

		dest = texAdjust * shadowProj * shadowView;
	}

	void CalculateSpotMatrix(Matrix4& dest, Light* light)
	{
		Node* lightNode = light->GetNode();
		Matrix3x4 spotView = Matrix3x4(lightNode->GetWorldPosition(), lightNode->GetWorldRotation(), 1.0f).Inverse();
		Matrix4 spotProj(Matrix4::ZERO);
		Matrix4 texAdjust(Matrix4::IDENTITY);

		// Make the projected light slightly smaller than the shadow map to prevent light spill
		float h = 1.005f / tanf(light->GetFov() * M_DEGTORAD * 0.5f);
		float w = h / light->GetAspectRatio();
		spotProj.m00_ = w;
		spotProj.m11_ = h;
		spotProj.m22_ = 1.0f / Max(light->GetRange(), M_EPSILON);
		spotProj.m32_ = 1.0f;

		if (GetSubsystem<Graphics>().IsOpenGL())
		{
			texAdjust.SetTranslation(Vector3(0.5f, 0.5f, 0.5f));
			texAdjust.SetScale(Vector3(0.5f, -0.5f, 0.5f));
		}
		else
		{
			texAdjust.SetTranslation(Vector3(0.5f, 0.5f, 0.0f));
			texAdjust.SetScale(Vector3(0.5f, -0.5f, 1.0f));
		}

		dest = texAdjust * spotProj * spotView;
	}

	/// Construct from a drawable's source batch.
	Batch::Batch(const SourceBatch& rhs) :
		distance_(rhs.distance_),
		pass_(0),
		geometry_(rhs.geometry_),
		material_(rhs.material_),
		worldTransform_(rhs.worldTransform_),
		numWorldTransforms_(rhs.numWorldTransforms_),
		instancingData_(rhs.instancingData_),
		geometryType_(rhs.geometryType_),
		zone_(nullptr)
	{
		assert(worldTransform_);
		primitiveTopology_ = rhs.primitiveTopology_;
		vertexOffset_ = rhs.vertexOffset_;
		vertexCount_ = rhs.vertexCount_;
		indexOffset_ = rhs.indexOffset_;
		indexCount_ = rhs.indexCount_;
	}

	/// Construct from transient buffer.
	Batch::Batch(Geometry* geometry, Material* material, const Matrix3x4* worldTransform) :
		pass_(0),
		geometry_(geometry),
		material_(material),
		worldTransform_(worldTransform),
		numWorldTransforms_(1),
		instancingData_(nullptr),
		geometryType_(GEOM_STATIC),
		zone_(nullptr)
	{
		if (material)
		{
			pipelineState_ = material->GetPipeline(ShaderUtil::basePassIndex, "");
		}

	}

	void Batch::SetDrawRange(PrimitiveTopology type, unsigned vertexStart, unsigned vertexCount)
	{
		geometryType_ = GEOM_TRANSIENT;
		primitiveTopology_ = type;
		vertexOffset_ = vertexStart;
		vertexCount_ = vertexCount;
		indexOffset_ = 0;
		indexCount_ = -1;
	}

	void Batch::SetDrawRange(PrimitiveTopology type, unsigned indexStart, unsigned indexCount, unsigned vertexStart, unsigned vertexCount)
	{
		geometryType_ = GEOM_TRANSIENT;
		primitiveTopology_ = type;
		vertexOffset_ = vertexStart;
		vertexCount_ = vertexCount;
		indexOffset_ = indexStart;
		indexCount_ = indexCount;
	}
	
	void Batch::CalculateSortKey()
	{
		unsigned shaderID = *((unsigned*)&pipelineState_) / sizeof(PipelineState);
		if (pass_ == 0)
			shaderID |= 0x8000;

//		unsigned lightQueueID = (unsigned)((*((unsigned*)&lightQueue_) / sizeof(LightBatchQueue)) & 0xffff);
		unsigned materialID = (unsigned)((*((unsigned*)&material_) / sizeof(Material)) & 0xffff);
		unsigned geometryID = (unsigned)((*((unsigned*)&geometry_) / sizeof(Geometry)) & 0xffff);

		sortKey_ = (((unsigned long long)shaderID) << 48) | /*(((unsigned long long)lightQueueID) << 32) |*/
			(((unsigned long long)materialID) << 16) | geometryID;
	}

	void Batch::Prepare(View* view, bool setModelTransform) const
	{
		auto& graphics = GetSubsystem<Graphics>();
		auto& renderer = GetSubsystem<Renderer>();

	//	Node* cameraNode = camera ? camera->GetNode() : 0;
		//Light* light = lightQueue_ ? lightQueue_->light_ : 0;
		//Texture* shadowMap = lightQueue_ ? lightQueue_->shadowMap_ : 0;
							
		// Set model or skinning transforms
		if (setModelTransform && view->NeedParameterUpdate(SP_OBJECT, (void*)transformOffset_))
		{
			if (geometryType_ == GEOM_SKINNED)
			{
				view->SetSkinedTransform(transformOffset_, numWorldTransforms_);
			}
			else
			{	
				view->SetWorldTransform(transformOffset_);
			}

			// Set the orientation for billboards, either from the object itself or from the camera
			//if (geometryType_ == GEOM_BILLBOARD)
			//{
			//	if (numWorldTransforms_ > 1)
			//		view->SetShaderParameter(VSP_BILLBOARDROT, worldTransform_[1].RotationMatrix());
			//	else
			//		view->SetShaderParameter(VSP_BILLBOARDROT, cameraNode->GetWorldRotation().RotationMatrix());
			//}
		}

			
		if (zone_ && view->NeedParameterUpdate(SP_ZONE, zone_))
		{
			view->SetZoneShaderParameters(zone_);
		}
		
#if false
		
		// Set zone-related shader parameters
		BlendMode blend = graphics->GetBlendMode();
		// If the pass is additive, override fog color to black so that shaders do not need a separate additive path
		bool overrideFogColorToBlack = blend == BLEND_ADD || blend == BLEND_ADDALPHA;
		unsigned zoneHash = (unsigned)(size_t)zone_;
		if (overrideFogColorToBlack)
			zoneHash += 0x80000000;
		if (zone_ && graphics->NeedParameterUpdate(SP_ZONE, reinterpret_cast<const void*>(zoneHash)))
		{
			graphics->SetShaderParameter(VSP_AMBIENTSTARTCOLOR, zone_->GetAmbientStartColor());
			graphics->SetShaderParameter(VSP_AMBIENTENDCOLOR,
				zone_->GetAmbientEndColor().ToVector4() - zone_->GetAmbientStartColor().ToVector4());

			const BoundingBox& box = zone_->GetBoundingBox();
			Vector3 boxSize = box.Size();
			Matrix3x4 adjust(Matrix3x4::IDENTITY);
			adjust.SetScale(Vector3(1.0f / boxSize.x_, 1.0f / boxSize.y_, 1.0f / boxSize.z_));
			adjust.SetTranslation(Vector3(0.5f, 0.5f, 0.5f));
			Matrix3x4 zoneTransform = adjust * zone_->GetInverseWorldTransform();
			graphics->SetShaderParameter(VSP_ZONE, zoneTransform);

			graphics->SetShaderParameter(PSP_AMBIENTCOLOR, zone_->GetAmbientColor());
			graphics->SetShaderParameter(PSP_FOGCOLOR, overrideFogColorToBlack ? Color::BLACK : zone_->GetFogColor());
			graphics->SetShaderParameter(PSP_ZONEMIN, zone_->GetBoundingBox().min_);
			graphics->SetShaderParameter(PSP_ZONEMAX, zone_->GetBoundingBox().max_);

			float farClip = camera->GetFarClip();
			float fogStart = Min(zone_->GetFogStart(), farClip);
			float fogEnd = Min(zone_->GetFogEnd(), farClip);
			if (fogStart >= fogEnd * (1.0f - M_LARGE_EPSILON))
				fogStart = fogEnd * (1.0f - M_LARGE_EPSILON);
			float fogRange = Max(fogEnd - fogStart, M_EPSILON);
			Vector4 fogParams(fogEnd / farClip, farClip / fogRange, 0.0f, 0.0f);

			Node* zoneNode = zone_->GetNode();
			if (zone_->GetHeightFog() && zoneNode)
			{
				Vector3 worldFogHeightVec = zoneNode->GetWorldTransform() * Vector3(0.0f, zone_->GetFogHeight(), 0.0f);
				fogParams.z_ = worldFogHeightVec.y_;
				fogParams.w_ = zone_->GetFogHeightScale() / Max(zoneNode->GetWorldScale().y_, M_EPSILON);
			}

			graphics->SetShaderParameter(PSP_FOGPARAMS, fogParams);
		}

		// Set light-related shader parameters
		if (lightQueue_)
		{
			if (light && graphics->NeedParameterUpdate(SP_LIGHT, lightQueue_))
			{
				Node* lightNode = light->GetNode();
				float atten = 1.0f / Max(light->GetRange(), M_EPSILON);
				Vector3 lightDir(lightNode->GetWorldRotation() * Vector3::BACK);
				Vector4 lightPos(lightNode->GetWorldPosition(), atten);

				graphics->SetShaderParameter(VSP_LIGHTDIR, lightDir);
				graphics->SetShaderParameter(VSP_LIGHTPOS, lightPos);

				if (graphics->HasShaderParameter(VSP_LIGHTMATRICES))
				{
					switch (light->GetLightType())
					{
					case LIGHT_DIRECTIONAL:
					{
						Matrix4 shadowMatrices[MAX_CASCADE_SPLITS];
						unsigned numSplits = Min(MAX_CASCADE_SPLITS, lightQueue_->shadowSplits_.Size());

						for (unsigned i = 0; i < numSplits; ++i)
							CalculateShadowMatrix(shadowMatrices[i], lightQueue_, i, renderer);

						graphics->SetShaderParameter(VSP_LIGHTMATRICES, shadowMatrices[0].Data(), 16 * numSplits);
					}
					break;

					case LIGHT_SPOT:
					{
						Matrix4 shadowMatrices[2];

						CalculateSpotMatrix(shadowMatrices[0], light);
						bool isShadowed = shadowMap && graphics->HasTextureUnit(TU_SHADOWMAP);
						if (isShadowed)
							CalculateShadowMatrix(shadowMatrices[1], lightQueue_, 0, renderer);

						graphics->SetShaderParameter(VSP_LIGHTMATRICES, shadowMatrices[0].Data(), isShadowed ? 32 : 16);
					}
					break;

					case LIGHT_POINT:
					{
						Matrix4 lightVecRot(lightNode->GetWorldRotation().RotationMatrix());
						// HLSL compiler will pack the parameters as if the matrix is only 3x4, so must be careful to not overwrite
						// the next parameter
#ifdef UNIQUE_OPENGL
						graphics->SetShaderParameter(VSP_LIGHTMATRICES, lightVecRot.Data(), 16);
#else
						graphics->SetShaderParameter(VSP_LIGHTMATRICES, lightVecRot.Data(), 12);
#endif
					}
					break;
					}
				}

				float fade = 1.0f;
				float fadeEnd = light->GetDrawDistance();
				float fadeStart = light->GetFadeDistance();

				// Do fade calculation for light if both fade & draw distance defined
				if (light->GetLightType() != LIGHT_DIRECTIONAL && fadeEnd > 0.0f && fadeStart > 0.0f && fadeStart < fadeEnd)
					fade = Min(1.0f - (light->GetDistance() - fadeStart) / (fadeEnd - fadeStart), 1.0f);

				// Negative lights will use subtract blending, so write absolute RGB values to the shader parameter
				graphics->SetShaderParameter(PSP_LIGHTCOLOR, Color(light->GetEffectiveColor().Abs(),
					light->GetEffectiveSpecularIntensity()) * fade);
				graphics->SetShaderParameter(PSP_LIGHTDIR, lightDir);
				graphics->SetShaderParameter(PSP_LIGHTPOS, lightPos);
				graphics->SetShaderParameter(PSP_LIGHTRAD, light->GetRadius());
				graphics->SetShaderParameter(PSP_LIGHTLENGTH, light->GetLength());

				if (graphics->HasShaderParameter(PSP_LIGHTMATRICES))
				{
					switch (light->GetLightType())
					{
					case LIGHT_DIRECTIONAL:
					{
						Matrix4 shadowMatrices[MAX_CASCADE_SPLITS];
						unsigned numSplits = Min(MAX_CASCADE_SPLITS, lightQueue_->shadowSplits_.Size());

						for (unsigned i = 0; i < numSplits; ++i)
							CalculateShadowMatrix(shadowMatrices[i], lightQueue_, i, renderer);

						graphics->SetShaderParameter(PSP_LIGHTMATRICES, shadowMatrices[0].Data(), 16 * numSplits);
					}
					break;

					case LIGHT_SPOT:
					{
						Matrix4 shadowMatrices[2];

						CalculateSpotMatrix(shadowMatrices[0], light);
						bool isShadowed = lightQueue_->shadowMap_ != 0;
						if (isShadowed)
							CalculateShadowMatrix(shadowMatrices[1], lightQueue_, 0, renderer);

						graphics->SetShaderParameter(PSP_LIGHTMATRICES, shadowMatrices[0].Data(), isShadowed ? 32 : 16);
					}
					break;

					case LIGHT_POINT:
					{
						Matrix4 lightVecRot(lightNode->GetWorldRotation().RotationMatrix());
						// HLSL compiler will pack the parameters as if the matrix is only 3x4, so must be careful to not overwrite
						// the next parameter
#ifdef UNIQUE_OPENGL
						graphics->SetShaderParameter(PSP_LIGHTMATRICES, lightVecRot.Data(), 16);
#else
						graphics->SetShaderParameter(PSP_LIGHTMATRICES, lightVecRot.Data(), 12);
#endif
					}
					break;
					}
				}

				// Set shadow mapping shader parameters
				if (shadowMap)
				{
					{
						// Calculate point light shadow sampling offsets (unrolled cube map)
						unsigned faceWidth = (unsigned)(shadowMap->GetWidth() / 2);
						unsigned faceHeight = (unsigned)(shadowMap->GetHeight() / 3);
						float width = (float)shadowMap->GetWidth();
						float height = (float)shadowMap->GetHeight();
#ifdef UNIQUE_OPENGL
						float mulX = (float)(faceWidth - 3) / width;
						float mulY = (float)(faceHeight - 3) / height;
						float addX = 1.5f / width;
						float addY = 1.5f / height;
#else
						float mulX = (float)(faceWidth - 4) / width;
						float mulY = (float)(faceHeight - 4) / height;
						float addX = 2.5f / width;
						float addY = 2.5f / height;
#endif
						// If using 4 shadow samples, offset the position diagonally by half pixel
						if (renderer->GetShadowQuality() == SHADOWQUALITY_PCF_16BIT || renderer->GetShadowQuality() == SHADOWQUALITY_PCF_24BIT)
						{
							addX -= 0.5f / width;
							addY -= 0.5f / height;
						}
						graphics->SetShaderParameter(PSP_SHADOWCUBEADJUST, Vector4(mulX, mulY, addX, addY));
					}

					{
						// Calculate shadow camera depth parameters for point light shadows and shadow fade parameters for
						//  directional light shadows, stored in the same uniform
						Camera* shadowCamera = lightQueue_->shadowSplits_[0].shadowCamera_;
						float nearClip = shadowCamera->GetNearClip();
						float farClip = shadowCamera->GetFarClip();
						float q = farClip / (farClip - nearClip);
						float r = -q * nearClip;

						const CascadeParameters& parameters = light->GetShadowCascade();
						float viewFarClip = camera->GetFarClip();
						float shadowRange = parameters.GetShadowRange();
						float fadeStart = parameters.fadeStart_ * shadowRange / viewFarClip;
						float fadeEnd = shadowRange / viewFarClip;
						float fadeRange = fadeEnd - fadeStart;

						graphics->SetShaderParameter(PSP_SHADOWDEPTHFADE, Vector4(q, r, fadeStart, 1.0f / fadeRange));
					}

					{
						float intensity = light->GetShadowIntensity();
						float fadeStart = light->GetShadowFadeDistance();
						float fadeEnd = light->GetShadowDistance();
						if (fadeStart > 0.0f && fadeEnd > 0.0f && fadeEnd > fadeStart)
							intensity =
							Lerp(intensity, 1.0f, Clamp((light->GetDistance() - fadeStart) / (fadeEnd - fadeStart), 0.0f, 1.0f));
						float pcfValues = (1.0f - intensity);
						float samples = 1.0f;
						if (renderer->GetShadowQuality() == SHADOWQUALITY_PCF_16BIT || renderer->GetShadowQuality() == SHADOWQUALITY_PCF_24BIT)
							samples = 4.0f;
						graphics->SetShaderParameter(PSP_SHADOWINTENSITY, Vector4(pcfValues / samples, intensity, 0.0f, 0.0f));
					}

					float sizeX = 1.0f / (float)shadowMap->GetWidth();
					float sizeY = 1.0f / (float)shadowMap->GetHeight();
					graphics->SetShaderParameter(PSP_SHADOWMAPINVSIZE, Vector2(sizeX, sizeY));

					Vector4 lightSplits(M_LARGE_VALUE, M_LARGE_VALUE, M_LARGE_VALUE, M_LARGE_VALUE);
					if (lightQueue_->shadowSplits_.Size() > 1)
						lightSplits.x_ = lightQueue_->shadowSplits_[0].farSplit_ / camera->GetFarClip();
					if (lightQueue_->shadowSplits_.Size() > 2)
						lightSplits.y_ = lightQueue_->shadowSplits_[1].farSplit_ / camera->GetFarClip();
					if (lightQueue_->shadowSplits_.Size() > 3)
						lightSplits.z_ = lightQueue_->shadowSplits_[2].farSplit_ / camera->GetFarClip();

					graphics->SetShaderParameter(PSP_SHADOWSPLITS, lightSplits);

					if (graphics->HasShaderParameter(PSP_VSMSHADOWPARAMS))
						graphics->SetShaderParameter(PSP_VSMSHADOWPARAMS, renderer->GetVSMShadowParameters());

					if (light->GetShadowBias().normalOffset_ > 0.0f)
					{
						Vector4 normalOffsetScale(Vector4::ZERO);

						// Scale normal offset strength with the width of the shadow camera view
						if (light->GetLightType() != LIGHT_DIRECTIONAL)
						{
							Camera* shadowCamera = lightQueue_->shadowSplits_[0].shadowCamera_;
							normalOffsetScale.x_ = 2.0f * tanf(shadowCamera->GetFov() * M_DEGTORAD * 0.5f) * shadowCamera->GetFarClip();
						}
						else
						{
							normalOffsetScale.x_ = lightQueue_->shadowSplits_[0].shadowCamera_->GetOrthoSize();
							if (lightQueue_->shadowSplits_.Size() > 1)
								normalOffsetScale.y_ = lightQueue_->shadowSplits_[1].shadowCamera_->GetOrthoSize();
							if (lightQueue_->shadowSplits_.Size() > 2)
								normalOffsetScale.z_ = lightQueue_->shadowSplits_[2].shadowCamera_->GetOrthoSize();
							if (lightQueue_->shadowSplits_.Size() > 3)
								normalOffsetScale.w_ = lightQueue_->shadowSplits_[3].shadowCamera_->GetOrthoSize();
						}

						normalOffsetScale *= light->GetShadowBias().normalOffset_;
#ifdef GL_ES_VERSION_2_0
						normalOffsetScale *= renderer->GetMobileNormalOffsetMul();
#endif
						graphics->SetShaderParameter(VSP_NORMALOFFSETSCALE, normalOffsetScale);
						graphics->SetShaderParameter(PSP_NORMALOFFSETSCALE, normalOffsetScale);
					}
				}
			}
			
		}

		// Set zone texture if necessary
#ifndef GL_ES_VERSION_2_0
		if (zone_ && graphics->HasTextureUnit(TU_ZONE))
			graphics->SetTexture(TU_ZONE, zone_->GetZoneTexture());
#else
	// On OpenGL ES set the zone texture to the environment unit instead
		if (zone_ && zone_->GetZoneTexture() && graphics->HasTextureUnit(TU_ENVIRONMENT))
			graphics->SetTexture(TU_ENVIRONMENT, zone_->GetZoneTexture());
#endif

		// Set material-specific shader parameters and textures
		if (material_)
		{
			if (graphics->NeedParameterUpdate(SP_MATERIAL, reinterpret_cast<const void*>(material_->GetShaderParameterHash())))
			{
				const HashMap<StringHash, MaterialShaderParameter>& parameters = material_->GetShaderParameters();
				for (HashMap<StringHash, MaterialShaderParameter>::ConstIterator i = parameters.Begin(); i != parameters.End(); ++i)
					graphics->SetShaderParameter(i->first_, i->second_.value_);
			}

			const HashMap<TextureUnit, SharedPtr<Texture> >& textures = material_->GetTextures();
			for (HashMap<TextureUnit, SharedPtr<Texture> >::ConstIterator i = textures.Begin(); i != textures.End(); ++i)
			{
				if (graphics->HasTextureUnit(i->first_))
					graphics->SetTexture(i->first_, i->second_.Get());
			}
		}

		// Set light-related textures
		if (light)
		{
			if (shadowMap && graphics->HasTextureUnit(TU_SHADOWMAP))
				graphics->SetTexture(TU_SHADOWMAP, shadowMap);
			if (graphics->HasTextureUnit(TU_LIGHTRAMP))
			{
				Texture* rampTexture = light->GetRampTexture();
				if (!rampTexture)
					rampTexture = renderer->GetDefaultLightRamp();
				graphics->SetTexture(TU_LIGHTRAMP, rampTexture);
			}
			if (graphics->HasTextureUnit(TU_LIGHTSHAPE))
			{
				Texture* shapeTexture = light->GetShapeTexture();
				if (!shapeTexture && light->GetLightType() == LIGHT_SPOT)
					shapeTexture = renderer->GetDefaultLightSpot();
				graphics->SetTexture(TU_LIGHTSHAPE, shapeTexture);
			}
		}
#endif
	}

	void Batch::Draw(View* view) const
	{		
		if (!geometry_->IsEmpty())
		{
			if(!pipelineState_->GetPipeline())
			{
				return;
			}
			
			Prepare(view, true);

			if (material_)
			{
				material_->Apply(pipelineState_);
			}

			auto& graphics = GetSubsystem<Graphics>();
			if (scissor_ != 0)
			{
				auto& scissor = GetSubsystem<Renderer>().GetScissor(scissor_);
				graphics.SetScissorRects(1, &scissor, 0, 0);
			}

			if (geometryType_ == GEOM_TRANSIENT )
			{
				graphics.Draw(geometry_, pipelineState_, primitiveTopology_,
					vertexOffset_, vertexCount_, indexOffset_, indexCount_);
			}
			else
			{
				graphics.Draw(geometry_, pipelineState_);
			}
		}
		
	}
	
	void Batch::Draw() const
	{
		if (!geometry_->IsEmpty())
		{
			if(!pipelineState_->GetPipeline())
			{
				return;
			}
			
			if (material_)
			{
				material_->Apply(pipelineState_);
			}

			auto& graphics = GetSubsystem<Graphics>();
			if (scissor_ != 0)
			{
				auto& scissor = GetSubsystem<Renderer>().GetScissor(scissor_);
				graphics.SetScissorRects(1, &scissor, 0, 0);
			}

			if (geometryType_ == GEOM_TRANSIENT )
			{
				graphics.Draw(geometry_, pipelineState_, primitiveTopology_, 
					vertexOffset_, vertexCount_, indexOffset_, indexCount_);
			}
			else
			{
				graphics.Draw(geometry_, pipelineState_);
			}
		}
		
	}

	void BatchGroup::SetInstancingData(void* lockedData, unsigned stride, unsigned& freeIndex)
	{
		// Do not use up buffer space if not going to draw as instanced
		//if (geometryType_ != GEOM_INSTANCED)
		//	return;

		startIndex_ = freeIndex;
		unsigned char* buffer = static_cast<unsigned char*>(lockedData) + startIndex_ * stride;

		for (size_t i = 0; i < instances_.size(); ++i)
		{
			const InstanceData& instance = instances_[i];

			memcpy(buffer, instance.worldTransform_, sizeof(Matrix3x4));
			if (instance.instancingData_)
				memcpy(buffer + sizeof(Matrix3x4), instance.instancingData_, stride - sizeof(Matrix3x4));

			buffer += stride;
		}

		freeIndex += (uint)instances_.size();
	}

	void BatchGroup::Draw(View* view) const
	{
		if (instances_.size() && !geometry_->IsEmpty())
		{
			if(!pipelineState_->GetPipeline())
			{
				return;
			}
			
			if (material_)
			{
				material_->Apply(pipelineState_);
			}
			
			auto& graphics = GetSubsystem<Graphics>();
			auto& renderer = GetSubsystem<Renderer>();
			// Draw as individual objects if instancing not supported or could not fill the instancing buffer
			VertexBuffer* instanceBuffer = renderer.GetInstancingBuffer();
			if (!instanceBuffer || geometryType_ != GEOM_INSTANCED || startIndex_ == M_MAX_UNSIGNED)
			{
				Batch::Prepare(view, false);
				
				for (unsigned i = 0; i < instances_.size(); ++i)
				{
					//if (view->NeedParameterUpdate(SP_OBJECT, instances_[i].worldTransform_))
					{
						view->SetWorldTransform(transformOffset_);
					}

					graphics.Draw(geometry_, pipelineState_);
				}
			}
			else
			{
				Batch::Prepare(view, false);

				// Get the geometry vertex buffers, then add the instancing stream buffer
				// Hack: use a const_cast to avoid dynamic allocation of new temp vectors
				Vector<SPtr<VertexBuffer> >& vertexBuffers = const_cast<Vector<SPtr<VertexBuffer> >&>(
					geometry_->GetVertexBuffers());
				vertexBuffers.push_back(SPtr<VertexBuffer>(instanceBuffer));
				
				graphics.DrawInstanced(geometry_, pipelineState_, startIndex_, (uint)instances_.size());
				// Remove the instancing buffer & element mask now
				vertexBuffers.pop_back();
			}
		}
	}

	unsigned BatchGroupKey::ToHash() const
	{
		return (unsigned)((size_t)lightQueue_ / sizeof(LightBatchQueue) + (size_t)pipelineState_ / sizeof(PipelineState) +
			(size_t)material_ / sizeof(Material) + (size_t)geometry_ / sizeof(Geometry)) + renderOrder_;
	}

	void BatchQueue::Clear(int maxSortedInstances)
	{
		batches_.clear();
		sortedBatches_.clear();
		batchGroups_.clear();
		sortedBatchGroups_.clear();
		maxSortedInstances_ = (unsigned)maxSortedInstances;
	}

	void BatchQueue::SortBackToFront()
	{
		sortedBatches_.resize(batches_.size());

		for (unsigned i = 0; i < batches_.size(); ++i)
			sortedBatches_[i] = &batches_[i];

		std::sort(sortedBatches_.begin(), sortedBatches_.end(), CompareBatchesBackToFront);

		sortedBatchGroups_.resize(batchGroups_.size());

		unsigned index = 0;
		for (auto i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
			sortedBatchGroups_[index++] = &i->second;

		std::sort(sortedBatchGroups_.begin(), sortedBatchGroups_.end(), CompareBatchGroupOrder);
	}

	void BatchQueue::SortFrontToBack()
	{
		sortedBatches_.clear();

		for (unsigned i = 0; i < batches_.size(); ++i)
			sortedBatches_.push_back(&batches_[i]);

		SortFrontToBack2Pass(sortedBatches_);

		// Sort each group front to back
		for (auto i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
		{
			if (i->second.instances_.size() <= maxSortedInstances_)
			{
				std::sort(i->second.instances_.begin(), i->second.instances_.end(), CompareInstancesFrontToBack);
				if (i->second.instances_.size())
					i->second.distance_ = i->second.instances_[0].distance_;
			}
			else
			{
				float minDistance = M_INFINITY;
				for (auto j = i->second.instances_.begin(); j != i->second.instances_.end(); ++j)
					minDistance = Min(minDistance, j->distance_);
				i->second.distance_ = minDistance;
			}
		}

		sortedBatchGroups_.resize(batchGroups_.size());

		unsigned index = 0;
		for (auto i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
			sortedBatchGroups_[index++] = &i->second;

		SortFrontToBack2Pass(reinterpret_cast<PODVector<Batch*>&>(sortedBatchGroups_));
	}

	void BatchQueue::SortFrontToBack2Pass(PODVector<Batch*>& batches)
	{
		std::sort(batches.begin(), batches.end(), CompareBatchesState);
	}

	void BatchQueue::SetInstancingData(void* lockedData, unsigned stride, unsigned& freeIndex)
	{
		for (auto i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
		{
			auto& batch = i->second;
			if (batch.geometryType_ == GEOM_STATIC)
			{
				continue;
			}

			batch.SetInstancingData(lockedData, stride, freeIndex);
		}
	}

	void BatchQueue::Draw(View* view) const
	{
		// Instanced
		//for (auto i = sortedBatchGroups_.begin(); i != sortedBatchGroups_.end(); ++i)
		for (int i = 0; i < sortedBatchGroups_.size(); ++i)
		{
			BatchGroup* group = sortedBatchGroups_[i];//*i;
			group->Draw(view);
		}
		// Non-instanced
		//for (auto i = sortedBatches_.begin(); i != sortedBatches_.end(); ++i)
		for (int i = 0; i < sortedBatches_.size(); ++i)
		{
			Batch* batch = sortedBatches_[i];//*i;
			batch->Draw(view);
		}
		
	}

	size_t BatchQueue::GetNumInstances(View* view)
	{
		size_t total = 0;

		for (auto i = batchGroups_.begin(); i != batchGroups_.end(); ++i)
		{
			auto& batch = i->second;
			if (batch.geometryType_ == GEOM_STATIC)
			{
				batch.transformOffset_ = view->GetMatrics(batch.worldTransform_, batch.numWorldTransforms_);
			}
			else
			{
				if (batch.geometryType_ == GEOM_INSTANCED)
					total += batch.instances_.size();
			}
		}

		return total;
	}
}