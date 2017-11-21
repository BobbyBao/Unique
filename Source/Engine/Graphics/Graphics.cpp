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

	Graphics::Graphics():
		profilerObj_ {	new LLGL::RenderingProfiler() },
		debuggerObj_{ new Debugger() }
	{
	}

	Graphics::~Graphics()
	{
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

		context = renderer->CreateRenderContext(contextDesc);

		// Create command buffer
		commands = renderer->CreateCommandBuffer();

		const LLGL::ColorRGBAf                      defaultClearColor{ 0.1f, 0.1f, 0.4f };

		// Initialize command buffer
		commands->SetClearColor(defaultClearColor);
		commands->SetRenderTarget(*context);
		commands->SetViewport({ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) });
		commands->SetScissor({ 0, 0, size.x, size.y });

		// Print renderer information
		const auto& info = renderer->GetRendererInfo();

		std::cout << "renderer information:" << std::endl;
		std::cout << "  renderer:         " << info.rendererName << std::endl;
		std::cout << "  device:           " << info.deviceName << std::endl;
		std::cout << "  vendor:           " << info.vendorName << std::endl;
		std::cout << "  shading language: " << info.shadingLanguageName << std::endl;


	}

}

