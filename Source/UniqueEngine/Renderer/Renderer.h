#pragma once
#include "Core/Object.h"
#include "../Graphics/Drawable.h"
#include "../Renderer/View.h"
#include "../Renderer/Viewport.h"

#include "../Graphics/VertexBuffer.h"

namespace Unique
{
	//class VertexBuffer;

	static const int INSTANCING_BUFFER_DEFAULT_SIZE = 1024;

	class Renderer : public Object
	{
		uRTTI(Renderer, Object)
	public:
		Renderer();
		~Renderer();

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

		void Begin();
		void Render();
		void End();
		void Stop();

		void DrawDebugGeometry(bool depthTest);
	private:
		void HandleEndFrame(const EndFrame& eventData);
		void HandleRenderUpdate(const RenderUpdate& eventData);
		void UpdateQueuedViewport(unsigned index);
		void QueueViewport(TextureView* renderTarget, Viewport* viewport);
		void CreateInstancingBuffer();
		bool ResizeInstancingBuffer(unsigned numInstances);

		class Graphics& graphics_;

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
	};

}

