#pragma once
#include "Core/Object.h"
#include "Batch.h"
#include "RenderPath.h"
#include "../Graphics/Light.h"
#include "../Graphics/Zone.h"

namespace Unique
{
	class Geometry;
	class Material;
	class PipelineState;
	class Renderer;
	class Viewport;
	class RenderPath;
	class ScenePass;
	class UniformBuffer;

	struct FrameParameter
	{
		float deltaTime_;
		float elapsedTime_;
	};
	
	struct CameraVS
	{
		Matrix3x4 view_;
		Matrix3x4 viewInv_;
		Matrix4 viewProj_;
		Vector3 cameraPos_;
		float nearClip_;
		float farClip_;
		Vector3 frustumSize_;
		Vector4 depthMode_;
		Vector4 gBufferOffsets_;
		Vector4 clipPlane_;
	};
	
	struct ObjectVS
	{
		Matrix3x4 world_;
	};

#define MAXBONES 64

	struct SkinnedVS
	{
		Matrix3x4 skinMatrices_[MAXBONES];
	};

	struct BillboardVS
	{
		Matrix3x4 world_;
		Matrix3 billboardRot;
	};

	struct MaterialVS
	{
		float4 UOffset;
		float4 VOffset;
	};

	//==================================================
	struct CameraPS
	{
		Vector3 cameraPosPS_;
		float dummy0;
		Vector4 depthReconstruct_;
		Vector2 gBufferInvSize_;
		float nearClipPS_;
		float farClipPS_;
	};

	struct ZonePS
	{
		Color ambientColor;
		Vector4 fogParams;
		Color fogColor;
		Vector3 zoneMin;
		Vector3 zoneMax;
	};

	struct LightPS
	{
		float4 LightColor;
		float4 LightPosPS;
		float3 LightDirPS;
		float4 NormalOffsetScalePS;
		float4 ShadowCubeAdjust;
		float4 ShadowDepthFade;
		float2 ShadowIntensity;
		float2 ShadowMapInvSize;
		float4 ShadowSplits;
		float2 VSMShadowParams;
		Matrix4 LightMatricesPS[4];
#ifdef PBR
	float LightRad;
	float LightLength;
#endif
	};

	struct MaterialPS
	{
		float4 matDiffColor;
		float3 matEmissiveColor;
		float3 matEnvMapColor;
		float4 matSpecColor;
		float roughness;
		float metallic;
	};

	/// Intermediate light processing result.
	struct LightQueryResult
	{
		/// Light.
		Light* light_;
		/// Lit geometries.
		PODVector<Drawable*> litGeometries_;
		/// Shadow casters.
		PODVector<Drawable*> shadowCasters_;
		/// Shadow cameras.
		Camera* shadowCameras_[MAX_LIGHT_SPLITS];
		/// Shadow caster start indices.
		unsigned shadowCasterBegin_[MAX_LIGHT_SPLITS];
		/// Shadow caster end indices.
		unsigned shadowCasterEnd_[MAX_LIGHT_SPLITS];
		/// Combined bounding box of shadow casters in light projection space. Only used for focused spot lights.
		BoundingBox shadowCasterBox_[MAX_LIGHT_SPLITS];
		/// Shadow camera near splits (directional lights only.)
		float shadowNearSplits_[MAX_LIGHT_SPLITS];
		/// Shadow camera far splits (directional lights only.)
		float shadowFarSplits_[MAX_LIGHT_SPLITS];
		/// Shadow map split count.
		unsigned numSplits_;
	};

	/// Scene render pass info.
	struct ScenePassInfo
	{
		/// Pass index.
		unsigned passIndex_ = 0;
		/// Allow instancing flag.
		bool allowInstancing_;
		/// Mark to stencil flag.
		bool markToStencil_;

		RenderPassSortMode sortMode_;
		/// Batch queue.
		BatchQueue* batchQueue_;
	};

	/// Per-thread geometry, light and scene range collection structure.
	struct PerThreadSceneResult
	{
		/// Geometry objects.
		PODVector<Drawable*> geometries_;
		/// Lights.
		PODVector<Light*> dirLights_;
		/// Lights.
		PODVector<Light*> pointLights_;
		/// Lights.
		PODVector<Light*> spotLights_;
		/// Scene minimum Z value.
		float minZ_;
		/// Scene maximum Z value.
		float maxZ_;
	};

	static const unsigned MAX_VIEWPORT_TEXTURES = 2;


	class View : public Object
	{
		uRTTI(View, Object)
	public:
		View();
		~View();

		bool Define(TextureView* renderTarget, Unique::Viewport* viewport);

		void Update(const FrameInfo& frame);
		void PostUpdate();
		void DrawDebug();

		void Render();

		/// Return scene.
		Scene* GetScene() const { return scene_; }

		/// Return octree.
		Octree* GetOctree() const { return octree_; }

		/// Return viewport camera.
		Camera* GetCamera() const { return camera_; }

		/// Return culling camera. Normally same as the viewport camera.
		Camera* GetCullCamera() const { return cullCamera_; }

		/// Return information of the frame being rendered.
		const FrameInfo& GetFrameInfo() const { return frame_; }

		/// Return the rendertarget. 0 if using the backbuffer.
		TextureView* GetRenderTarget() const { return renderTarget_; }

		/// Return whether should draw debug geometry.
		bool GetDrawDebug() const { return drawDebug_; }

		/// Return view rectangle.
		const IntRect& GetViewRect() const { return viewRect_; }

		/// Return view dimensions.
		const IntVector2& GetViewSize() const { return viewSize_; }

		/// Return geometry objects.
		const PODVector<Drawable*>& GetGeometries() const { return geometries_; }

		/// Return occluder objects.
		const PODVector<Drawable*>& GetOccluders() const { return occluders_; }

		/// Lights.
		PODVector<Light*> dirLights_;
		/// Lights.
		PODVector<Light*> pointLights_;
		/// Lights.
		PODVector<Light*> spotLights_;

		/// Return light batch queues.
//		const Vector<LightBatchQueue>& GetLightQueues() const { return lightQueues_; }
		  
		/// Check whether a shader parameter group needs update. Does not actually check whether parameters exist in the shaders.
		bool NeedParameterUpdate(ShaderParameterGroup group, const void* source);
		/// Clear remembered shader parameter source group.
		void ClearParameterSource(ShaderParameterGroup group);
		/// Clear remembered shader parameter sources.
		void ClearParameterSources();
		/// Clear remembered transform shader parameter sources.
		void ClearTransformSources();
		/// Set global (per-frame) shader parameters. Called by Batch and internally by View.
		void SetGlobalShaderParameters();
		/// Set camera-specific shader parameters. Called by Batch and internally by View.
		void SetCameraShaderParameters(Camera* camera);
		void SetZoneShaderParameters(Zone* zone);
		size_t GetMatrics(const Matrix3x4* transform, uint num);
		void SetWorldTransform(size_t offset);
		void SetSkinedTransform(size_t offset, uint numTransforms);

	protected:
		/// Query the octree for drawable objects.
		void GetDrawables();
		/// Construct batches from the drawable objects.
		void GetBatches();

		void GetBaseBatches();

		void AddBatchToQueue(BatchQueue& queue, Batch& batch, uint passIndex, bool allowInstancing = true, bool allowShadows = true);

		/// Prepare instancing buffer by filling it with all instance transforms.
		void PrepareInstancingBuffer();

		void UpdateGeometries();
		/// Query for lit geometries and shadow casters for a light.
		void ProcessLight(LightQueryResult& query, unsigned threadIndex);
		  
		/// Find and set a new zone for a drawable when it has moved.
		void FindZone(Drawable* drawable);
		
		/// Return the drawable's zone, or camera zone if it has override mode enabled.
		Zone* GetZone(Drawable* drawable)
		{
			if (cameraZoneOverride_)
				return cameraZone_;
			Zone* drawableZone = drawable->GetZone();
			return drawableZone ? drawableZone : cameraZone_;
		}

		/// Return the drawable's light mask, considering also its zone.
		unsigned GetLightMask(Drawable* drawable)
		{
			return drawable->GetLightMask() & GetZone(drawable)->GetLightMask();
		}

		/// Return the drawable's shadow mask, considering also its zone.
		unsigned GetShadowMask(Drawable* drawable)
		{
			return drawable->GetShadowMask() & GetZone(drawable)->GetShadowMask();
		}
		
		/// Graphics subsystem.
		Graphics& graphics_;
		/// Renderer subsystem.
		Renderer& renderer_;
		/// Scene to use.
		Scene* scene_;
		/// Octree to use.
		Octree* octree_;
		/// Viewport (rendering) camera.
		Camera* camera_;
		/// Culling camera. Usually same as the viewport camera.
		Camera* cullCamera_;
		/// Zone the camera is inside, or default zone if not assigned.
		Zone* cameraZone_;
		/// Zone at far clip plane.
		Zone* farClipZone_;
		/// Destination color rendertarget.
		TextureView* renderTarget_;
		/// Substitute rendertarget for deferred rendering. Allocated if necessary.
		TextureView* substituteRenderTarget_;
		/// Texture(s) for sampling the viewport contents. Allocated if necessary.
		Texture* viewportTextures_[MAX_VIEWPORT_TEXTURES];
		/// Color rendertarget active for the current renderpath command.
		TextureView* currentRenderTarget_;
		/// Last used custom depth render surface.
		TextureView* lastCustomDepthSurface_;
		/// Texture containing the latest viewport texture.
		Texture* currentViewportTexture_;
		/// Dummy texture for D3D9 depth only rendering.
		Texture* depthOnlyDummyTexture_;
		/// Viewport rectangle.
		IntRect viewRect_;
		/// Viewport size.
		IntVector2 viewSize_;
		/// Destination rendertarget size.
		IntVector2 rtSize_;
		/// Information of the frame being rendered.
		FrameInfo frame_;
		/// View aspect ratio.
		float aspectRatio_;
		/// Minimum Z value of the visible scene.
		float minZ_;
		/// Maximum Z value of the visible scene.
		float maxZ_;
		/// Minimum number of instances required in a batch group to render as instanced.
		int minInstances_;
		/// Highest zone priority currently visible.
		int highestZonePriority_;
		/// Geometries updated flag.
		bool geometriesUpdated_;
		/// Camera zone's override flag.
		bool cameraZoneOverride_;
		/// Draw shadows flag.
		bool drawShadows_;
		/// Has scene passes flag. If no scene passes, view can be defined without a valid scene or camera to only perform quad rendering.
		bool hasScenePasses_;
		/// Draw debug geometry flag. Copied from the viewport.
		bool drawDebug_;
		/// Renderpath.
		RenderPath* renderPath_;
		/// Per-thread octree query results.
		Vector<PODVector<Drawable*> > tempDrawables_;
		/// Per-thread geometries, lights and Z range collection results.
		Vector<PerThreadSceneResult> sceneResults_;
		/// Visible zones.
		PODVector<Zone*> zones_;
		/// Visible geometry objects.
		PODVector<Drawable*> geometries_;
		/// Geometry objects that will be updated in the main thread.
		PODVector<Drawable*> nonThreadedGeometries_;
		/// Geometry objects that will be updated in worker threads.
		PODVector<Drawable*> threadedGeometries_;
		/// Occluder objects.
		PODVector<Drawable*> occluders_;
		/// Number of active occluders.
		unsigned activeOccluders_;

		Vector<SPtr<RenderPass>> renderPasses_[2];
		/// Info for scene render passes defined by the renderpath.
		PODVector<ScenePassInfo> scenePasses_[2];
		/// Per-pixel light queues.
		Vector<LightBatchQueue> lightQueues_;
		/// Batch queues by pass index.
		HashMap<byte, BatchQueue> batchQueues_[2];
		
		/// Remembered shader parameter sources.
		const void* shaderParameterSources_[MAX_SHADER_PARAMETER_GROUPS];

		SPtr<UniformBuffer> frameUniform_;
		SPtr<UniformBuffer> cameraVS_;
		SPtr<UniformBuffer> objectVS_;
		SPtr<UniformBuffer> skinnedVS_;
		SPtr<UniformBuffer> billboardVS_;
		SPtr<UniformBuffer> materialVS_;
		SPtr<UniformBuffer> cameraPS_; 
		SPtr<UniformBuffer> zonePS_;
		SPtr<UniformBuffer> lightPS_;
		SPtr<UniformBuffer> materialPS_;

		Vector<Matrix3x4>	batchMatrics_[2];
		HashMap<const Matrix3x4*, size_t> matricsToOffset_;
				
		friend class ScenePass;
		friend class ZoneOctreeQuery;

		friend void CheckVisibilityWork(const WorkItem* item, unsigned threadIndex);
		friend void ProcessLightWork(const WorkItem* item, unsigned threadIndex);

	};

}

