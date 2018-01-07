#pragma once
#include "Core/Object.h"
#include "../Graphics/Drawable.h"
#include "../Renderer/View.h"
#include "../Renderer/Viewport.h"

#include "../Graphics/VertexBuffer.h"

namespace Unique
{
	class Geometry;
	struct Batch;

	static const int INSTANCING_BUFFER_DEFAULT_SIZE = 1024;

	struct TransientGeometry
	{
		Geometry* geometry_;
		uint vertexOffset_;
		uint vertexCount_;
		uint indexOffset_;
		uint indexCount_;
	};

	class Renderer : public Object
	{
		uRTTI(Renderer, Object)
	public:
		Renderer();
		~Renderer();

		/// Set number of extra instancing buffer elements. Default is 0. Extra 4-vectors are available through TEXCOORD7 and further.
		void SetNumExtraInstancingBufferElements(int elements);
		/// Set minimum number of instances required in a batch group to render as instanced.
		void SetMinInstances(int instances);
		/// Set maximum number of sorted instances per batch group. If exceeded, instances are rendered unsorted.
		void SetMaxSortedInstances(int instances);

		/// Set number of backbuffer viewports to render.
		void SetNumViewports(unsigned num);
		/// Set a backbuffer viewport.
		void SetViewport(unsigned index, Viewport* viewport);
		/// Return number of backbuffer viewports.
		size_t GetNumViewports() const { return viewports_.size(); }

		/// Return backbuffer viewport by index.
		Viewport* GetViewport(unsigned index) const;

		/// Return the instancing vertex buffer
		VertexBuffer* GetInstancingBuffer() const { return instancingBuffer_.Get(); }

		RenderPath* GetDefaultRenderPath() { return defaultRenderPath_; }

		/// Return number of extra instancing buffer elements.
		int GetNumExtraInstancingBufferElements() const { return numExtraInstancingBufferElements_; };

		/// Return minimum number of instances required in a batch group to render as instanced.
		int GetMinInstances() const { return minInstances_; }

		/// Return maximum number of sorted instances per batch group.
		int GetMaxSortedInstances() const { return maxSortedInstances_; }

		void Begin();
		void Render();
		void End();
		void Stop();
		void SetBatchShaders(Batch& batch, Shader* tech, bool allowShadows, const BatchQueue& queue);
		bool ResizeInstancingBuffer(unsigned numInstances);

		ushort CacheScissor(int left, int top, int right, int bottom);
		IntRect& GetScissor(ushort handler);

		Batch& AddBatch(Geometry* geometry, Material* material, const Matrix3x4* worldTransform = nullptr);
		Batch& AddBatch(const Batch& batch);
	private:
		/// Initialize when screen mode initially set.
		void Initialize();
		/// Reload shaders.
		void LoadShaders();
		void HandleEndFrame(const EndFrame& eventData);
		void HandleRenderUpdate(const RenderUpdate& eventData);
		void HandlePostRenderUpdate(const PostRenderUpdate& eventData);
		void UpdateQueuedViewport(unsigned index);
		void QueueViewport(TextureView* renderTarget, Viewport* viewport);
		void CreateInstancingBuffer();
		void DrawDebugGeometry(bool depthTest);

		class Graphics& graphics_;

		/// Default renderpath.
		SPtr<RenderPath> defaultRenderPath_;
		/// Default non-textured material technique.
		SPtr<Shader> defaultTechnique_;
		/// Backbuffer viewports.
		Vector<SPtr<Viewport> > viewports_;
		/// Render surface viewports queued for update.
		Vector<Pair<WPtr<TextureView>, WPtr<Viewport> > > queuedViewports_;
		/// Views that have been processed this frame.
		Vector<WPtr<View> > views_[2];
		/// Prepared views by culling camera.
		HashMap<Camera*, WPtr<View> > preparedViews_;
		/// Octrees that have been updated during the frame.
		HashSet<Octree*> updatedOctrees_;
		/// Frame info for rendering.
		FrameInfo frame_;    
		/// Instance stream vertex buffer.
		SPtr<VertexBuffer> instancingBuffer_;
		/// Number of extra instancing data elements.
		int numExtraInstancingBufferElements_;
		/// Minimum number of instances required in a batch group to render as instanced.
		int minInstances_;
		/// Maximum sorted instances per batch group.
		int maxSortedInstances_;
		
		SPtr<Geometry> transientGeometry_;

		SPtr<Geometry> transientIndexedGeometry_;
		
		bool drawDebug_;

		Vector<IntRect> scissorRect_[2];
		
		Vector<Batch> batchQueue_[2];
	};

}

