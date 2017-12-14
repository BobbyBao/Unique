#include "UniquePCH.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "Core/CoreEvents.h"

namespace Unique
{
	Renderer::Renderer() : graphics(GetSubsystem<Graphics>())
	{
		Subscribe(&Renderer::HandleEndFrame);
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

	void Renderer::HandleEndFrame(const EndFrame& args)
	{
		graphics.Frame();
	}

	void Renderer::Begin()
	{
		graphics.BeginRender();
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