#include "Precompiled.h"
#include "Graphics.h"

namespace Unique
{
	uObject(Graphics)
	{
		uFactory("Graphics")
	}

	class Debugger : public LLGL::RenderingDebugger
	{
		void OnError(LLGL::ErrorType type, Message& message) override
		{
			std::cerr << "ERROR: " << message.GetSource() << ": " << message.GetText() << std::endl;
			message.Block();
		}

		void OnWarning(LLGL::WarningType type, Message& message) override
		{
			std::cerr << "WARNING: " << message.GetSource() << ": " << message.GetText() << std::endl;
			message.Block();
		}

	};



	// Render system
	UPtr<LLGL::RenderSystem>        renderer;

	// Main render context
	LLGL::RenderContext*			graphicsContext = nullptr;

	// Main command buffer
	LLGL::CommandBuffer*            commands = nullptr;

	Graphics::Graphics():
		profilerObj_ {	new LLGL::RenderingProfiler() },
		debuggerObj_{ new Debugger() }
	{
	}

	Graphics::~Graphics()
	{
		if (renderer)
		{
			LLGL::RenderSystem::Unload(std::move(renderer));
		}
	}

	void Graphics::SetDebug(bool val)
	{
		debugger_ = val;
	}

	void Graphics::Initialize(const std::string& rendererModule, const LLGL::Size& size)
	{
		// Create render system
		renderer = LLGL::RenderSystem::Load(
			rendererModule,
			(debugger_ ? profilerObj_.get() : nullptr),
			(debugger_ ? debuggerObj_.get() : nullptr)
		);

		// Create render context
		LLGL::RenderContextDescriptor contextDesc;
		{
			contextDesc.videoMode.resolution = size;
			contextDesc.vsync.enabled = vsync_;
			contextDesc.multiSampling.enabled = (multiSampling_ > 1);
			contextDesc.multiSampling.samples = multiSampling_;

#if 0
			contextDesc.profileOpenGL.extProfile = true;
			contextDesc.profileOpenGL.coreProfile = true;
			contextDesc.profileOpenGL.version = LLGL::OpenGLVersion::OpenGL_3_3;
			/*contextDesc.debugCallback = [](const std::string& type, const std::string& message)
			{
			std::cout << type << " -- " << message << std::endl;
			};*/
#endif

#ifdef __linux__
			contextDesc.profileOpenGL.extProfile = true;
			contextDesc.profileOpenGL.coreProfile = true;
			contextDesc.profileOpenGL.version = LLGL::OpenGLVersion::OpenGL_3_3;
#endif
		}

		graphicsContext = renderer->CreateRenderContext(contextDesc);

		// Create command buffer
		commands = renderer->CreateCommandBuffer();

		const LLGL::ColorRGBAf                      defaultClearColor{ 0.1f, 0.1f, 0.4f };

		// Initialize command buffer
		commands->SetClearColor(defaultClearColor);
		commands->SetRenderTarget(*graphicsContext);
		commands->SetViewport({ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) });
		commands->SetScissor({ 0, 0, size.x, size.y });

		// Print renderer information
		const auto& info = renderer->GetRendererInfo();

		std::cout << "renderer information:" << std::endl;
		std::cout << "  renderer:         " << info.rendererName << std::endl;
		std::cout << "  device:           " << info.deviceName << std::endl;
		std::cout << "  vendor:           " << info.vendorName << std::endl;
		std::cout << "  shading language: " << info.shadingLanguageName << std::endl;

		FrameNoRenderWait();
	}


	void Graphics::AddCommand(std::function<void()> cmd)
	{
		preComands_.push_back(cmd);
	}

	void Graphics::PostCommand(std::function<void()> cmd)
	{
		postComands_.push_back(cmd);
	}

	void Graphics::BeginFrame()
	{
	}

	void Graphics::EndFrame()
	{
		RenderSemWait();

		FrameNoRenderWait();
	}


	void Graphics::SwapContext()
	{
	}

	void Graphics::FrameNoRenderWait()
	{
		SwapContext();

		// release render thread
		MainSemPost();
	}


	void Graphics::ExecuteCommands(CommandQueue& cmds)
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

	void Graphics::MainSemPost()
	{
		if (!singleThreaded_)
		{
			mainSem_.post();
		}
	}

	bool Graphics::MainSemWait(int _msecs)
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

	void Graphics::RenderSemPost()
	{
		if (!singleThreaded_)
		{
			renderSem_.post();
		}
	}

	void Graphics::RenderSemWait()
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

	RenderFrameResult Graphics::RenderFrame(int _msecs)
	{

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

	void Graphics::Flip()
	{
		graphicsContext->Present();
	}
}

