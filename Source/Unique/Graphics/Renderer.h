#pragma once
#include "../Core/Object.h"
#include "../Graphics/View.h"

namespace Unique
{
	class Renderer : public Object
	{
		uRTTI(Renderer, Object)
	public:
		Renderer();
		~Renderer();

		
	private:
		void Handle(StringID type, const EndFrame& args);

		void RenderUpdate();
		void Render();

		class Graphics& graphics;
		Vector<SPtr<View>> views_;
	};

}

