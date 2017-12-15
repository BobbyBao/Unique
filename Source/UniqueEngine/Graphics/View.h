#pragma once
#include "Core/Object.h"
#include "Batch.h"
#include "Light.h"

namespace Unique
{
	class Geometry;
	class Material;
	class PipelineState;

	struct ViewParameter
	{
		Matrix4 view_;
		Matrix4 viewInv_;
		Matrix4 viewProj_;
	};

	struct ObjectVS
	{
		Matrix4 world_;
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
		unsigned passIndex_;
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

		void Update(const FrameInfo& frame);

		void Render();
	protected:
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

		FrameInfo frame_;
		/// View aspect ratio.
		float aspectRatio_;
		/// Minimum Z value of the visible scene.
		float minZ_;
		/// Maximum Z value of the visible scene.
		float maxZ_;

		/// Draw debug geometry flag. Copied from the viewport.
		bool drawDebug_;
		/// Renderpath.
		//RenderPath* renderPath_;
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
	};

}

