#pragma once
#include "Core/Object.h"
#include "../Graphics/View.h"

namespace Unique
{
	class Renderer : public Object
	{
		uRTTI(Renderer, Object)
	public:
		Renderer();
		~Renderer();

		void Begin();
		void Render();
		void End();
		void Stop();
	private:
		void HandleEndFrame(const EndFrame& args);
		void RenderUpdate();

		class Graphics& graphics;
		Vector<SPtr<View>> views_;
	};

}

