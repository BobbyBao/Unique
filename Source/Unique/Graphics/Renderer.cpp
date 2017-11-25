#include "Precompiled.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "../Core/CoreEvents.h"

namespace Unique
{
	Renderer::Renderer()
	{
		SubscribeToEvent(E_ENDFRAME, &Renderer::Handle);
		FrameNoRenderWait();
	}


	Renderer::~Renderer()
	{
	}


	void Renderer::AddCommand(std::function<void()> cmd)
	{		
		preComands_.push_back(cmd);
	}

	void Renderer::PostCommand(std::function<void()> cmd)
	{
		postComands_.push_back(cmd);
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
		UpdateFrame();
	}

	void Renderer::UpdateFrame()
	{
		RenderSemWait();	
		
		FrameNoRenderWait();
	}

	void Renderer::SwapContext()
	{
	}

	void Renderer::FrameNoRenderWait()
	{
		SwapContext();

		// release render thread
		MainSemPost();
	}

	RenderFrameResult Renderer::RenderFrame(int _msecs)
	{
		for (auto& view : views_)
		{
			view->Render();
		}

		Flip();
	
		if (MainSemWait(_msecs))
		{
			{
				UNIQUE_PROFILE(ExecCommandsPre);
				ExecuteCommands(preComands_);
			}
			
			{
				UNIQUE_PROFILE(ExecCommandsPost);
				ExecuteCommands(postComands_);
			}

			RenderSemPost();

		}
		else
		{
			return RenderFrameResult::Timeout;
		}
	
		if (exit_)
		{
			MainSemWait();
			RenderSemPost();
			return RenderFrameResult::Exiting;
		}

		return RenderFrameResult::Render;
	}

	void Renderer::ExecuteCommands(CommandQueue& cmds)
	{
		if (!cmds.empty())
		{
			for (auto& fn : cmds)
			{
				fn();
			}

			cmds.clear();
		}
	}

	void Renderer::MainSemPost()
	{
		if (!singleThreaded_)
		{
			mainSem_.post();
		}
	}

	bool Renderer::MainSemWait(int _msecs)
	{
		if (singleThreaded_)
		{
			return true;
		}

		UNIQUE_PROFILE(MainThreadWait);
		HiresTimer timer;
		bool ok = mainSem_.wait(_msecs);
		if (ok)
		{
			waitSubmit_ = timer.GetUSec(false);
			return true;
		}

		return false;
	}

	void Renderer::RenderSemPost()
	{
		if (!singleThreaded_)
		{
			renderSem_.post();
		}
	}

	void Renderer::RenderSemWait()
	{
		if (!singleThreaded_)
		{
			UNIQUE_PROFILE(RenderThreadWait);
			HiresTimer timer;
			bool ok = renderSem_.wait();
			assert(ok);
			waitRender_ = timer.GetUSec(false);
		}
	}
	
	void Renderer::Flip()
	{
		graphicsContext->Present();
	}
}