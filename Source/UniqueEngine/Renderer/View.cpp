#include "UniquePCH.h"
#include "Core/WorkQueue.h"
#include "../Scene/Scene.h"
#include "../Graphics/GraphicsBuffer.h"
#include "../Graphics/Geometry.h"
#include "../Graphics/Octree.h"
#include "../Graphics/OctreeQuery.h"
#include "../Graphics/Camera.h"
#include "Viewport.h"
#include "View.h"
#include "Batch.h"
#include "RenderPath.h"
#include "ScenePass.h"

namespace Unique
{
	static const Vector3* directions[] =
	{
		&Vector3::RIGHT,
		&Vector3::LEFT,
		&Vector3::UP,
		&Vector3::DOWN,
		&Vector3::FORWARD,
		&Vector3::BACK
	};

	/// %Frustum octree query for shadowcasters.
	class ShadowCasterOctreeQuery : public FrustumOctreeQuery
	{
	public:
		/// Construct with frustum and query parameters.
		ShadowCasterOctreeQuery(PODVector<Drawable*>& result, const Frustum& frustum, unsigned char drawableFlags = DRAWABLE_ANY,
			unsigned viewMask = DEFAULT_VIEWMASK) :
			FrustumOctreeQuery(result, frustum, drawableFlags, viewMask)
		{
		}

		/// Intersection test for drawables.
		virtual void TestDrawables(Drawable** start, Drawable** end, bool inside)
		{
			while (start != end)
			{
				Drawable* drawable = *start++;

				if (drawable->GetCastShadows() && (drawable->GetDrawableFlags() & drawableFlags_) &&
					(drawable->GetViewMask() & viewMask_))
				{
					if (inside || frustum_.IsInsideFast(drawable->GetWorldBoundingBox()))
						result_.push_back(drawable);
				}
			}
		}
	};

	void CheckVisibilityWork(const WorkItem* item, unsigned threadIndex)
	{
		View* view = reinterpret_cast<View*>(item->aux_);
		Drawable** start = reinterpret_cast<Drawable**>(item->start_);
		Drawable** end = reinterpret_cast<Drawable**>(item->end_);
		const Matrix3x4& viewMatrix = view->cullCamera_->GetView();
		Vector3 viewZ = Vector3(viewMatrix.m20_, viewMatrix.m21_, viewMatrix.m22_);
		Vector3 absViewZ = viewZ.Abs();
		unsigned cameraViewMask = view->cullCamera_->GetViewMask();
		PerThreadSceneResult& result = view->sceneResults_[threadIndex];

		while (start != end)
		{
			Drawable* drawable = *start++;

			//if (!drawable->IsOccludee())
			{
				drawable->UpdateBatches(view->frame_);
				// If draw distance non-zero, update and check it
				float maxDistance = drawable->GetDrawDistance();
				if (maxDistance > 0.0f)
				{
					if (drawable->GetDistance() > maxDistance)
						continue;
				}

				drawable->MarkInView(view->frame_);

				// For geometries, find zone, clear lights and calculate view space Z range
				if (drawable->GetDrawableFlags() & DRAWABLE_GEOMETRY)
				{
					const BoundingBox& geomBox = drawable->GetWorldBoundingBox();
					Vector3 center = geomBox.Center();
					Vector3 edge = geomBox.Size() * 0.5f;

					// Do not add "infinite" objects like skybox to prevent shadow map focusing behaving erroneously
					if (edge.LengthSquared() < M_LARGE_VALUE * M_LARGE_VALUE)
					{
						float viewCenterZ = viewZ.DotProduct(center) + viewMatrix.m23_;
						float viewEdgeZ = absViewZ.DotProduct(edge);
						float minZ = viewCenterZ - viewEdgeZ;
						float maxZ = viewCenterZ + viewEdgeZ;
						drawable->SetMinMaxZ(viewCenterZ - viewEdgeZ, viewCenterZ + viewEdgeZ);
						result.minZ_ = Min(result.minZ_, minZ);
						result.maxZ_ = Max(result.maxZ_, maxZ);
					}
					else
						drawable->SetMinMaxZ(M_LARGE_VALUE, M_LARGE_VALUE);

					result.geometries_.push_back(drawable);
				}
				else if (drawable->GetDrawableFlags() & DRAWABLE_LIGHT)
				{
					Light* light = static_cast<Light*>(drawable);
					// Skip lights with zero brightness or black color
					if (!light->GetEffectiveColor().Equals(Color::BLACK))
						result.lights_.push_back(light);
				}
			}
		}
	}

	void ProcessLightWork(const WorkItem* item, unsigned threadIndex)
	{
		View* view = reinterpret_cast<View*>(item->aux_);
		LightQueryResult* query = reinterpret_cast<LightQueryResult*>(item->start_);

		view->ProcessLight(*query, threadIndex);
	}

	void UpdateDrawableGeometriesWork(const WorkItem* item, unsigned threadIndex)
	{
		const FrameInfo& frame = *(reinterpret_cast<FrameInfo*>(item->aux_));
		Drawable** start = reinterpret_cast<Drawable**>(item->start_);
		Drawable** end = reinterpret_cast<Drawable**>(item->end_);

		while (start != end)
		{
			Drawable* drawable = *start++;
			// We may leave null pointer holes in the queue if a drawable is found out to require a main thread update
			if (drawable)
				drawable->UpdateGeometry(frame);
		}
	}

	void SortBatchQueueFrontToBackWork(const WorkItem* item, unsigned threadIndex)
	{
		BatchQueue* queue = reinterpret_cast<BatchQueue*>(item->start_);

		queue->SortFrontToBack();
	}

	void SortBatchQueueBackToFrontWork(const WorkItem* item, unsigned threadIndex)
	{
		BatchQueue* queue = reinterpret_cast<BatchQueue*>(item->start_);

		queue->SortBackToFront();
	}

	void SortLightQueueWork(const WorkItem* item, unsigned threadIndex)
	{
		LightBatchQueue* start = reinterpret_cast<LightBatchQueue*>(item->start_);
		start->litBaseBatches_.SortFrontToBack();
		start->litBatches_.SortFrontToBack();
	}

	void SortShadowQueueWork(const WorkItem* item, unsigned threadIndex)
	{
		LightBatchQueue* start = reinterpret_cast<LightBatchQueue*>(item->start_);
		for (unsigned i = 0; i < start->shadowSplits_.size(); ++i)
			start->shadowSplits_[i].shadowBatches_.SortFrontToBack();
	}

	View::View() 
		: graphics_(GetSubsystem<Graphics>()),
		renderer_(GetSubsystem<Renderer>()), 
		geometriesUpdated_(false), minInstances_(2)
	{
		frameUniform_ = new UniformBuffer(FrameParameter());
		cameraUniformVS_ = new UniformBuffer(CameraVS());
		objectUniformVS_ = new UniformBuffer(ObjectVS());

		cameraUniformPS_ = new UniformBuffer(CameraPS());
		materialUniformPS_ = new UniformBuffer(MaterialPS());

		tempDrawables_.resize(1);

		batchMatrics_[0].reserve(2048);
		batchMatrics_[1].reserve(2048);

		graphics_.AddResource("FrameVS", frameUniform_);
		graphics_.AddResource("CameraVS", cameraUniformVS_);
		graphics_.AddResource("ObjectVS", objectUniformVS_);

		graphics_.AddResource("CameraPS", cameraUniformPS_);
		graphics_.AddResource("MaterialPS", materialUniformPS_);
	}

	View::~View()
	{
	}

	bool View::Define(TextureView* renderTarget, Unique::Viewport* viewport)
	{
 		renderPath_ = viewport->GetRenderPath();
 		if (!renderPath_)
 			return false;

		renderTarget_ = renderTarget;
		drawDebug_ = viewport->GetDrawDebug();

		// Validate the rect and calculate size. If zero rect, use whole rendertarget size
		int rtWidth = renderTarget ? renderTarget->GetWidth() : graphics_.GetWidth();
		int rtHeight = renderTarget ? renderTarget->GetHeight() : graphics_.GetHeight();
		const IntRect& rect = viewport->GetRect();

		if (rect != IntRect::ZERO)
		{
			viewRect_.left_ = Clamp(rect.left_, 0, rtWidth - 1);
			viewRect_.top_ = Clamp(rect.top_, 0, rtHeight - 1);
			viewRect_.right_ = Clamp(rect.right_, viewRect_.left_ + 1, rtWidth);
			viewRect_.bottom_ = Clamp(rect.bottom_, viewRect_.top_ + 1, rtHeight);
		}
		else
			viewRect_ = IntRect(0, 0, rtWidth, rtHeight);

		viewSize_ = viewRect_.Size();
		rtSize_ = IntVector2(rtWidth, rtHeight);

		// On OpenGL flip the viewport if rendering to a texture for consistent UV addressing with Direct3D9
		if (graphics_.IsOpenGL())
		{
			if (renderTarget_)
			{
				viewRect_.bottom_ = rtHeight - viewRect_.top_;
				viewRect_.top_ = viewRect_.bottom_ - viewSize_.y_;
			}
		}

		scene_ = viewport->GetScene();
		octree_ = scene_->GetComponent<Octree>();

		cullCamera_ = viewport->GetCullCamera();
		camera_ = viewport->GetCamera();
		if (!cullCamera_)
			cullCamera_ = camera_;

		auto& scenePasses = MainContext(scenePasses_);
		hasScenePasses_ = false;
		scenePasses.clear();
		geometriesUpdated_ = false;

		auto& batchQueues = MainContext(batchQueues_);

		auto& renderPasses = renderPath_->GetRenderPasses();
		for (auto& pass : renderPasses)
		{
			if (pass->type_ == RenderPassType::SCENEPASS)
			{
				hasScenePasses_ = true;

				ScenePassInfo info;
				info.passIndex_ = pass->passIndex_ = Shader::GetPassIndex(pass->pass_);
				info.allowInstancing_ = pass->sortMode_ != RenderPassSortMode::BACKTOFRONT;

				auto j = batchQueues.find(info.passIndex_);
				if (j == batchQueues.end())
				{
					auto k = batchQueues.insert(Pair<unsigned, BatchQueue>(info.passIndex_, BatchQueue()));
					info.batchQueue_ = &(k.first->second);
				}
				else
				{
					info.batchQueue_ = &j->second;
				}

				scenePasses.push_back(info);

			}
		}

		if (hasScenePasses_)
		{
			if (!scene_ || !camera_ || !camera_->IsEnabledEffective())
				return false;

			if (!octree_)
				return false;

			if (!camera_->IsProjectionValid())
				return false;
		}

		return true;
	}

	void View::Update(const FrameInfo& frame)
	{
		frame_.camera_ = camera_;
		frame_.timeStep_ = frame.timeStep_;
		frame_.frameNumber_ = frame.frameNumber_;
		frame_.viewSize_ = viewSize_;

		int maxSortedInstances = renderer_.GetMaxSortedInstances();

	//	renderTargets_.Clear();
		geometries_.clear();
		lights_.clear();

		auto& batchQueues = MainContext(batchQueues_);
		for (auto i = batchQueues.begin(); i != batchQueues.end(); ++i)
			i->second.Clear(maxSortedInstances);

		if (hasScenePasses_ && (!camera_ || !octree_))
		{
		//	SendViewEvent(E_ENDVIEWUPDATE);
			return;
		}

		if (camera_ && camera_->GetAutoAspectRatio())
			camera_->SetAspectRatioInternal((float)frame_.viewSize_.x_ / (float)frame_.viewSize_.y_);
				
		GetDrawables();
		GetBatches();

		UpdateGeometries();
		
		PrepareInstancingBuffer();

		SetCameraShaderParameters(camera_);
	}

	void View::Render()
	{
		renderPath_->Render(this);
	}

	void View::SetGlobalShaderParameters()
	{
		FrameParameter frameParam;
		frameParam.deltaTime_ = frame_.timeStep_;

		if (scene_)
		{
			float elapsedTime = scene_->GetElapsedTime();
			frameParam.elapsedTime_ = elapsedTime;
		}

		frameUniform_->SetData(frameParam);

	}

	void View::SetCameraShaderParameters(Camera* camera)
	{
		if (!camera)
			return;

		float nearClip = camera->GetNearClip();
		float farClip = camera->GetFarClip();
		Matrix3x4 cameraEffectiveTransform = camera->GetEffectiveWorldTransform();

		{
			Vector4 depthMode = Vector4::ZERO;
			if (camera->IsOrthographic())
			{
				depthMode.x_ = 1.0f;
				if (graphics_.IsOpenGL())
				{
					depthMode.z_ = 0.5f;
					depthMode.w_ = 0.5f;
				}
				else
				{
					depthMode.z_ = 1.0f;
				}

			}
			else
				depthMode.w_ = 1.0f / camera->GetFarClip();


			Vector3 nearVector, farVector;
			camera->GetFrustumSize(nearVector, farVector);

			Matrix4 projection = camera->GetGPUProjection();

			if (graphics_.IsOpenGL())
			{
				// Add constant depth bias manually to the projection matrix due to glPolygonOffset() inconsistency
				//float constantBias = 2.0f * graphics_->GetDepthConstantBias();
				//projection.m22_ += projection.m32_ * constantBias;
				//projection.m23_ += projection.m33_ * constantBias;
			}
			
			CameraVS* cameraParam = (CameraVS*)cameraUniformVS_->Lock();
			cameraParam->cameraPos_ = cameraEffectiveTransform.Translation();
			cameraParam->nearClip_ = nearClip;
			cameraParam->farClip_ = farClip;
			cameraParam->depthMode_ = depthMode;
			cameraParam->frustumSize_ = farVector;
			cameraParam->view_ = camera->GetView();
			cameraParam->viewInv_ = cameraEffectiveTransform;
			cameraParam->viewProj_ = projection * camera->GetView();

			cameraUniformVS_->Unlock();

		}

		{
			CameraPS* cameraParam = (CameraPS*)cameraUniformPS_->Lock();
			cameraParam->cameraPosPS_ = cameraEffectiveTransform.Translation();

			cameraParam->nearClipPS_ = nearClip;
			cameraParam->farClipPS_ = farClip;

			Vector4 depthReconstruct
			(farClip / (farClip - nearClip), -nearClip / (farClip - nearClip), camera->IsOrthographic() ? 1.0f : 0.0f,
				camera->IsOrthographic() ? 0.0f : 1.0f);
			cameraParam->depthReconstruct_ = depthReconstruct;
			cameraUniformPS_->Unlock();
		}

		{
			ObjectVS* data = (ObjectVS*)objectUniformVS_->Lock();
			data->world_ = Matrix3x4::IDENTITY;
			objectUniformVS_->Unlock();
		}
		{
			MaterialPS* data = (MaterialPS*)materialUniformPS_->Lock();
			data->matDiffColor = float4(1,1,1,1);
			materialUniformPS_->Unlock();
		}
	}

	void View::GetDrawables()
	{
		if (!octree_ || !cullCamera_)
			return;

		UNIQUE_PROFILE(GetDrawables);

		auto& queue = GetSubsystem<WorkQueue>();

		PODVector<Drawable*>& tempDrawables = tempDrawables_[0];
		FrustumOctreeQuery query(tempDrawables, cullCamera_->GetFrustum(), DRAWABLE_GEOMETRY | DRAWABLE_LIGHT, cullCamera_->GetViewMask());
		octree_->GetDrawables(query);
		std::swap(geometries_, tempDrawables);
	}

	void View::GetBatches()
	{
		if (!octree_ || !cullCamera_)
			return;

		nonThreadedGeometries_.clear();
		threadedGeometries_.clear();

// 		ProcessLights();
// 		GetLightBatches();

 		GetBaseBatches();
	}

	void View::GetBaseBatches()
	{
		UNIQUE_PROFILE(GetBaseBatches);

		for (auto i = geometries_.begin(); i != geometries_.end(); ++i)
		{
			Drawable* drawable = *i;
			UpdateGeometryType type = drawable->GetUpdateGeometryType();
			if (type == UPDATE_MAIN_THREAD)
				nonThreadedGeometries_.push_back(drawable);
			else if (type == UPDATE_WORKER_THREAD)
				threadedGeometries_.push_back(drawable);

			const Vector<SourceBatch>& batches = drawable->GetBatches();
			bool vertexLightsProcessed = false;

			auto& scenePasses = MainContext(scenePasses_);

			for (unsigned j = 0; j < batches.size(); ++j)
			{
				const SourceBatch& srcBatch = batches[j];
				Shader* shader = srcBatch.material_->GetShader();
				if (!srcBatch.geometry_ || !srcBatch.numWorldTransforms_ || !shader)
					continue;

				// Check each of the scene passes
				for (unsigned k = 0; k < scenePasses.size(); ++k)
				{
					ScenePassInfo& info = scenePasses[k];
					// Skip forward base pass if the corresponding litbase pass already exists
// 					if (info.passIndex_ == basePassIndex_ && j < 32 && drawable->HasBasePass(j))
// 						continue;

					Pass* pass = shader->GetPass(info.passIndex_);
					if (!pass)
						continue;

					Batch destBatch(srcBatch);
					destBatch.pass_ = pass;
					destBatch.isBase_ = true;
					//destBatch.lightMask_ = (unsigned char)GetLightMask(drawable);
					destBatch.lightQueue_ = 0;

					AddBatchToQueue(*info.batchQueue_, destBatch, shader, info.allowInstancing_);
				}
			}
		}
	}


	void View::AddBatch(Batch& batch)
	{
		assert(batch.pass_ != nullptr);

		auto& batchQueues = MainContext(batchQueues_);
		auto it = batchQueues.find(batch.pass_->passIndex_);
		if (it != batchQueues.end())
		{
			it->second.batches_.push_back(batch);
		}
		else
		{
			UNIQUE_LOGERROR("Unknown pass index : ", batch.pass_->passIndex_);
		}
	}

	void View::AddBatchToQueue(BatchQueue& queue, Batch& batch, Shader* shader, bool allowInstancing, bool allowShadows)
	{
		//if (!batch.material_)
		//	batch.material_ = renderer_->GetDefaultMaterial();

		// Convert to instanced if possible
		if (allowInstancing && batch.geometryType_ == GEOM_STATIC && batch.geometry_->GetIndexBuffer())
			batch.geometryType_ = GEOM_INSTANCED;

		if (batch.geometryType_ == GEOM_INSTANCED)
		{
			BatchGroupKey key(batch);

			auto i = queue.batchGroups_.find(key);
			if (i == queue.batchGroups_.end())
			{
				// Create a new group based on the batch
				// In case the group remains below the instancing limit, do not enable instancing shaders yet
				BatchGroup newGroup(batch);
				newGroup.geometryType_ = GEOM_STATIC;
				renderer_.SetBatchShaders(newGroup, shader, allowShadows, queue);
				newGroup.CalculateSortKey();
				newGroup.AddTransforms(batch);
				queue.batchGroups_.insert(std::make_pair(key, newGroup));
			}
			else
			{
				size_t oldSize = i->second.instances_.size();
				i->second.AddTransforms(batch);
				// Convert to using instancing shaders when the instancing limit is reached
				if (oldSize < minInstances_ && (int)i->second.instances_.size() >= minInstances_)
				{
					i->second.geometryType_ = GEOM_INSTANCED;
					renderer_.SetBatchShaders(i->second, shader, allowShadows, queue);
					i->second.CalculateSortKey();
				}
			}
		}
		else
		{
			renderer_.SetBatchShaders(batch, shader, allowShadows, queue);
			batch.CalculateSortKey();

			// If batch is static with multiple world transforms and cannot instance, we must push copies of the batch individually
			if (batch.geometryType_ == GEOM_STATIC && batch.numWorldTransforms_ > 1)
			{
				unsigned numTransforms = batch.numWorldTransforms_;
				batch.numWorldTransforms_ = 1;
				for (unsigned i = 0; i < numTransforms; ++i)
				{
					// Move the transform pointer to generate copies of the batch which only refer to 1 world transform
					queue.batches_.push_back(batch);
					auto& b = queue.batches_.back();
					b.transformOffset_ = GetMatrics(&b.worldTransform_[i], 1);
					++batch.worldTransform_;
				}
			}
			else
			{
				queue.batches_.push_back(batch);	
				auto& b = queue.batches_.back();
				b.transformOffset_ = GetMatrics(b.worldTransform_, b.numWorldTransforms_);
			}
		}
	}

	size_t View::GetMatrics(const Matrix3x4* transform, uint num)
	{
		auto& batchMatrics = MainContext(batchMatrics_);
		size_t offset = batchMatrics.size();
		size_t newSize = offset + num;
		size_t cap = batchMatrics.capacity();
		if (newSize > cap)
		{
			batchMatrics.reserve(newSize);
		}

		batchMatrics.resize(newSize);
		std::memcpy(&batchMatrics[offset], transform, num * sizeof(Matrix3x4));

		return offset;
	}

	void View::SetWorldTransform(size_t offset)
	{
		auto& batchMatrics = RenderContext(batchMatrics_);
		const Matrix3x4* mat  = &batchMatrics[offset];
		void* data = objectUniformVS_->Map();
		std::memcpy(data, mat, sizeof(Matrix3x4));
		objectUniformVS_->UnMap();
	}

	void View::SetSkinedTransform(size_t offset, uint count)
	{
		auto& batchMatrics = RenderContext(batchMatrics_);
		const Matrix3x4* mat = &batchMatrics[offset];
		void* data = objectUniformVS_->Map();
		std::memcpy(data, mat, sizeof(Matrix3x4) * count);
		objectUniformVS_->UnMap();
	}

	void View::PrepareInstancingBuffer()
	{
		UNIQUE_PROFILE(PrepareInstancingBuffer);

		size_t totalInstances = 0;

		auto& batchQueues = MainContext(batchQueues_);

		for (auto i = batchQueues.begin(); i != batchQueues.end(); ++i)
			totalInstances += i->second.GetNumInstances(this);
		/*
		for (auto i = lightQueues_.begin(); i != lightQueues_.end(); ++i)
		{
			for (unsigned j = 0; j < i->shadowSplits_.size(); ++j)
				totalInstances += i->shadowSplits_[j].shadowBatches_.GetNumInstances();
			totalInstances += i->litBaseBatches_.GetNumInstances();
			totalInstances += i->litBatches_.GetNumInstances();
		}*/

		if (!totalInstances || !renderer_.ResizeInstancingBuffer((uint)totalInstances))
			return;

		VertexBuffer* instancingBuffer = renderer_.GetInstancingBuffer();
		unsigned freeIndex = 0;
		void* dest = instancingBuffer->Lock(0, (uint)totalInstances);
		if (!dest)
			return;

		const unsigned stride = instancingBuffer->GetStride();
		for (auto i = batchQueues.begin(); i != batchQueues.end(); ++i)
		{
			i->second.SetInstancingData(dest, stride, freeIndex);
		}

		/*
		for (auto i = lightQueues_.begin(); i != lightQueues_.end(); ++i)
		{
			for (unsigned j = 0; j < i->shadowSplits_.size(); ++j)
				i->shadowSplits_[j].shadowBatches_.SetInstancingData(dest, stride, freeIndex);
			i->litBaseBatches_.SetInstancingData(dest, stride, freeIndex);
			i->litBatches_.SetInstancingData(dest, stride, freeIndex);
		}*/

		instancingBuffer->Unlock();
	}

	void View::UpdateGeometries()
	{
		UNIQUE_PROFILE(SortAndUpdateGeometry);

		WorkQueue& queue = GetSubsystem<WorkQueue>();

		// Sort batches
		{
			auto& renderPasses = renderPath_->GetRenderPasses();
			for (unsigned i = 0; i < renderPasses.size(); ++i)
			{
				const auto& command = renderPasses[i];
				//if (!IsNecessary(command))
				//	continue;

				if (command->type_ == RenderPassType::SCENEPASS)
				{
					SPtr<WorkItem> item = queue.GetFreeItem();
					item->priority_ = M_MAX_UNSIGNED;
					item->workFunction_ =
						command->sortMode_ == RenderPassSortMode::FRONTTOBACK ? SortBatchQueueFrontToBackWork : SortBatchQueueBackToFrontWork;
					item->start_ = &MainContext(batchQueues_)[command->passIndex_];
					queue.AddWorkItem(item);
				}
			}
#if false
			for (auto i = lightQueues_.Begin(); i != lightQueues_.End(); ++i)
			{
				SPtr<WorkItem> lightItem = queue->GetFreeItem();
				lightItem->priority_ = M_MAX_UNSIGNED;
				lightItem->workFunction_ = SortLightQueueWork;
				lightItem->start_ = &(*i);
				queue->AddWorkItem(lightItem);

				if (i->shadowSplits_.Size())
				{
					SPtr<WorkItem> shadowItem = queue->GetFreeItem();
					shadowItem->priority_ = M_MAX_UNSIGNED;
					shadowItem->workFunction_ = SortShadowQueueWork;
					shadowItem->start_ = &(*i);
					queue->AddWorkItem(shadowItem);
				}
			}
#endif
		}


		renderPath_->Update(this);

		// Update geometries. Split into threaded and non-threaded updates.
		{
			if (threadedGeometries_.size())
			{
				// In special cases (context loss, multi-view) a drawable may theoretically first have reported a threaded update, but will actually
				// require a main thread update. Check these cases first and move as applicable. The threaded work routine will tolerate the null
				// pointer holes that we leave to the threaded update queue.
				for (auto i = threadedGeometries_.begin(); i != threadedGeometries_.end(); ++i)
				{
					if ((*i)->GetUpdateGeometryType() == UPDATE_MAIN_THREAD)
					{
						nonThreadedGeometries_.push_back(*i);
						*i = 0;
					}
				}

				int numWorkItems = (int)queue.GetNumThreads() + 1; // Worker threads + main thread
				int drawablesPerItem = (int)threadedGeometries_.size() / numWorkItems;

				auto start = threadedGeometries_.begin();
				for (int i = 0; i < numWorkItems; ++i)
				{
					auto end = threadedGeometries_.end();
					if (i < numWorkItems - 1 && end - start > drawablesPerItem)
						end = start + drawablesPerItem;

					SPtr<WorkItem> item = queue.GetFreeItem();
					item->priority_ = M_MAX_UNSIGNED;
					item->workFunction_ = UpdateDrawableGeometriesWork;
					item->aux_ = const_cast<FrameInfo*>(&frame_);
					item->start_ = &(*start);
					item->end_ = &(*end);
					queue.AddWorkItem(item);

					start = end;
				}
			}

			// While the work queue is processed, update non-threaded geometries
			for (auto i = nonThreadedGeometries_.begin(); i != nonThreadedGeometries_.end(); ++i)
				(*i)->UpdateGeometry(frame_);
		}

		// Finally ensure all threaded work has completed
		queue.Complete(M_MAX_UNSIGNED);
		geometriesUpdated_ = true;
	}

	/// Query for lit geometries and shadow casters for a light.
	void View::ProcessLight(LightQueryResult& query, unsigned threadIndex)
	{

	}

}

