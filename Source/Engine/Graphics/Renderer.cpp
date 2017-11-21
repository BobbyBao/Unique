#include "Precompiled.h"
#include "Renderer.h"


namespace Unique
{
	Renderer::Renderer()
	{
	}


	Renderer::~Renderer()
	{
	}

	void Renderer::ThreadFunction()
	{

	}

	RenderFrameResult Renderer::RenderFrame(int _msecs)
	{
		RenderFrameResult result = RenderFrameResult::NoContext;

		//if (!m_flipAfterRender)
		{
		//	BGFX_PROFILER_SCOPE("bgfx/flip", 0xff2040ff);
		//	flip();
		}

		if (apiSemWait(_msecs))
		{
			{
			//	BGFX_PROFILER_SCOPE("bgfx/Exec commands pre", 0xff2040ff);
				ExecuteCommands(preComands_);
			}

			//if (m_rendererInitialized)
			{
			//	BGFX_PROFILER_SCOPE("bgfx/Render submit", 0xff2040ff);
			//	m_renderCtx->submit(m_render, m_clearQuad, m_textVideoMemBlitter);
			//	m_flipped = false;
			}

			{
			//	BGFX_PROFILER_SCOPE("bgfx/Exec commands post", 0xff2040ff);
				ExecuteCommands(postComands_);
			}

			renderSemPost();

// 			if (m_flipAfterRender)
// 			{
// 				BGFX_PROFILER_SCOPE("bgfx/flip", 0xff2040ff);
// 				flip();
// 			}
		}
		else
		{
			return RenderFrameResult::Timeout;
		}
		/*
		return m_exit
			? RenderFrameResult::Exiting
			: RenderFrameResult::Render
			;


		if (NULL == s_ctx)
		{
			s_renderFrameCalled = true;
			s_threadIndex = ~BGFX_MAIN_THREAD_MAGIC;
			return RenderFrameResult::NoContext;
		}

		int32_t msecs = -1 == _msecs
			? BGFX_CONFIG_API_SEMAPHORE_TIMEOUT
			: _msecs
			;
		RenderFrameResult result = s_ctx->renderFrame(msecs);
		if (RenderFrameResult::Exiting == result)
		{
			Context* ctx = s_ctx;
			ctx->apiSemWait();
			s_ctx = NULL;
			ctx->renderSemPost();
		}*/

		return result;
	}

	void Renderer::ExecuteCommands(CommandQueue& cmds)
	{
		for (auto& fn : cmds)
		{
			fn();
		}
	}

	void Renderer::apiSemPost()
	{
		if (!m_singleThreaded)
		{
			m_apiSem.post();
		}
	}

	bool Renderer::apiSemWait(int32_t _msecs)
	{
		if (m_singleThreaded)
		{
			return true;
		}

		//BGFX_PROFILER_SCOPE("bgfx/API thread wait", 0xff2040ff);
		//int64_t start = bx::getHPCounter();
		bool ok = m_apiSem.wait(_msecs);
		if (ok)
		{
			//	m_render->m_waitSubmit = bx::getHPCounter() - start;
			//	m_submit->m_perfStats.waitSubmit = m_submit->m_waitSubmit;
			return true;
		}

		return false;
	}

	void Renderer::renderSemPost()
	{
		if (!m_singleThreaded)
		{
			m_renderSem.post();
		}
	}

	void Renderer::renderSemWait()
	{
		if (!m_singleThreaded)
		{
			//	BGFX_PROFILER_SCOPE("bgfx/Render thread wait", 0xff2040ff);
			//	int64_t start = bx::getHPCounter();
			bool ok = m_renderSem.wait();
			//	BX_CHECK(ok, "Semaphore wait failed."); BX_UNUSED(ok);
			//	m_submit->m_waitRender = bx::getHPCounter() - start;
			//	m_submit->m_perfStats.waitRender = m_submit->m_waitRender;
		}
	}
}