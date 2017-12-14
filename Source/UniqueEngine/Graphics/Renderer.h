#pragma once
#include "Core/Object.h"
#include "../Graphics/View.h"
#include "../Graphics/Drawable.h"
#include "../Graphics/Viewport.h"

namespace Unique
{
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

		void Begin();
		void Render();
		void End();
		void Stop();
	private:
		void HandleEndFrame(const EndFrame& eventData);
		void HandleRenderUpdate(const RenderUpdate& eventData);

		class Graphics& graphics;

		/// Backbuffer viewports.
		Vector<SPtr<Viewport> > viewports_;

		Vector<SPtr<View>> views_;  
		
		/// Frame info for rendering.
		FrameInfo frame_;
	};

}

