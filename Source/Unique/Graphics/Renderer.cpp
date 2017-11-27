#include "Precompiled.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "../Core/CoreEvents.h"

namespace Unique
{
	Renderer::Renderer() : graphics(Subsystem<Graphics>())
	{
		SubscribeToEvent(&Renderer::Handle);

	}


	Renderer::~Renderer()
	{
	}


	void Renderer::RenderUpdate()
	{
		for (auto& view : views_)
		{
			view->Update();
		}

	}

	void Renderer::Handle(StringID type, const EndFrame& args)
	{
		graphics.EndFrame();
	}


	void Renderer::Begin()
	{
		//	profilerObj_->ResetCounters();
	}

	void Renderer::Render()
	{
		for (auto& view : views_)
		{
			view->Render();
		}

	}

	void Renderer::End()
	{
		graphics.EndRender();
	}

	void Renderer::Stop()
	{
		graphics.Close();
	}
}