#include "UniquePCH.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "Core/CoreEvents.h"

namespace Unique
{
	Renderer::Renderer() : graphics(GetSubsystem<Graphics>())
	{
		Subscribe(&Renderer::HandleRenderUpdate);
		Subscribe(&Renderer::HandleEndFrame);
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::SetNumViewports(unsigned num)
	{
		viewports_.resize(num);
	}

	void Renderer::SetViewport(unsigned index, Viewport* viewport)
	{
		if (index >= viewports_.size())
			viewports_.resize(index + 1);

		viewports_[index] = viewport;
	}

	Viewport* Renderer::GetViewport(unsigned index) const
	{
		return index < viewports_.size() ? viewports_[index] : (Viewport*)0;
	}

	void Renderer::HandleRenderUpdate(const RenderUpdate& eventData)
	{  
		// Set up the frameinfo structure for this frame
		frame_.frameNumber_ = GetSubsystem<Time>().GetFrameNumber();
		frame_.timeStep_ = eventData.timeStep_;
		frame_.camera_ = 0;

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