#pragma once
#include "Core/Object.h"
#include "Batch.h"
#include "../Graphics/Light.h"

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
		Vector4 ambientColor;
		Vector4 fogParams;
		Vector3 fogColor;
		Vector3 zoneMin;
		Vector3 zoneMax;
	};


	struct ObjectVS
	{
		Matrix3x4 world_;
#ifdef BILLBOARD
		float3x3 cBillboardRot;
#endif
#ifdef SKINNED
		uniform Matrix3x4 cSkinMatrices[MAXBONES];
#endif
	};

	struct MaterialPS //: register(b4)
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
		/// Vertex light flag.
		bool vertexLights_;
		/// Batch queue.
		BatchQueue* batchQueue_;
	};

	/// Per-thread geometry, light and scene range collection structure.
	struct PerThreadSceneResult
	{
		/// Geometry objects.
		PODVector<Drawable*> geometries_;
		/// Lights.
		PODVector<Light*> lights_;
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

		/// Return lights.
		const PODVector<Light*>& GetLights() const { return lights_; }

		/// Return light batch queues.
//		const Vector<LightBatchQueue>& GetLightQueues() const { return lightQueues_; }

		/// Set global (per-frame) shader parameters. Called by Batch and internally by View.
		void SetGlobalShaderParameters();
		/// Set camera-specific shader parameters. Called by Batch and internally by View.
		void SetCameraShaderParameters(Camera* camera);

		void AddBatch(Batch& batch);
	protected:
		/// Query the octree for drawable objects.
		void GetDrawables();
		/// Construct batches from the drawable objects.
		void GetBatches();

		void GetBaseBatches();

		void AddBatchToQueue(BatchQueue& queue, Batch& batch, Shader* tech, bool allowInstancing = true, bool allowShadows = true);
		
		uint GetMatrics(const Matrix3x4* transform, uint num);

		/// Prepare instancing buffer by filling it with all instance transforms.
		void PrepareInstancingBuffer();

		void UpdateGeometries();
		/// Query for lit geometries and shadow casters for a light.
		void ProcessLight(LightQueryResult& query, unsigned threadIndex);
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
		/// Geometries updated flag.
		bool geometriesUpdated_;
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
		/// Lights.
		PODVector<Light*> lights_;
		/// Number of active occluders.
		unsigned activeOccluders_;
		/// Info for scene render passes defined by the renderpath.
		PODVector<ScenePassInfo> scenePasses_[2];
		/// Per-pixel light queues.
		Vector<LightBatchQueue> lightQueues_;
		/// Batch queues by pass index.
		HashMap<byte, BatchQueue> batchQueues_[2];

		SPtr<UniformBuffer> frameUniform_;

		SPtr<UniformBuffer> cameraUniformVS_;
		SPtr<UniformBuffer> objectUniformVS_;

		SPtr<UniformBuffer> cameraUniformPS_;
		SPtr<UniformBuffer> materialUniformPS_;

		Vector<Matrix3x4>	batchMatrics_[2];

		Vector<Matrix3x4>	batchGroupMatrics_[2];
		
		friend class ScenePass;

		friend void CheckVisibilityWork(const WorkItem* item, unsigned threadIndex);
		friend void ProcessLightWork(const WorkItem* item, unsigned threadIndex);

	};

}

