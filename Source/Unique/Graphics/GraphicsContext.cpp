#include "Precompiled.h"
#include "GraphicsContext.h"
#include "Core/Timer.h"

namespace Unique
{
	bool GraphicsContext::singleThreaded_ = false;

	Semaphore GraphicsContext::renderSem_;

	Semaphore GraphicsContext::mainSem_;

	long long GraphicsContext::waitSubmit_ = 0;

	long long GraphicsContext::waitRender_ = 0;

	Vector < std::function<void()>> GraphicsContext::comands_;;

	int GraphicsContext::currentContext_ = 0;

	GraphicsContext::GraphicsContext()
	{
	}

	GraphicsContext::~GraphicsContext()
	{
	}

	void GraphicsContext::AddCommand(std::function<void()> cmd)
	{
		assert(Context::IsMainThread());
		comands_.push_back(cmd);
	}

	void GraphicsContext::ExecuteCommands(CommandQueue& cmds)
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
	
	void GraphicsContext::BeginFrame()
	{
	}
	
	void GraphicsContext::EndFrame()
	{
		RenderSemWait();

		FrameNoRenderWait();
	}

	void GraphicsContext::BeginRender()
	{
//		profilerObj_->ResetCounters();
	}

	void GraphicsContext::EndRender()
	{
		if (MainSemWait())
		{
			{
				UNIQUE_PROFILE(ExecCommands);
				ExecuteCommands(comands_);
			}

			SwapContext();

			RenderSemPost();

		}
	}

	void GraphicsContext::Stop()
	{
		MainSemWait();
		//FrameNoRenderWait();
		RenderSemPost();
	}

	void GraphicsContext::SwapContext()
	{
		currentContext_ = 1 - currentContext_;
	}

	void GraphicsContext::FrameNoRenderWait()
	{
		// release render thread
		MainSemPost();
	}

	void GraphicsContext::MainSemPost()
	{
		if (!singleThreaded_)
		{
			mainSem_.post();
		}
	}

	bool GraphicsContext::MainSemWait(int _msecs)
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

	void GraphicsContext::RenderSemPost()
	{
		if (!singleThreaded_)
		{
			renderSem_.post();
		}
	}

	void GraphicsContext::RenderSemWait()
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
}
