#include "Precompiled.h"
#include "Application.h"
#include "../IO/FileSystem.h"
#include <iostream>
#include <fstream>
#include "../Graphics/Graphics.h"

namespace Unique
{


	class ResizeEventHandler : public LLGL::Window::EventListener
	{
	public:
		ResizeEventHandler(Application& application,
			LLGL::RenderContext* context,
			LLGL::CommandBuffer* commands,
			Gs::Matrix4f& projection) :
			application_{ application },
			context_{ context },
			commands_{ commands },
			projection_{ projection }
		{
		}

		void OnResize(LLGL::Window& sender, const LLGL::Size& clientAreaSize) override
		{
			auto videoMode = context_->GetVideoMode();

			// Update video mode
			videoMode.resolution = clientAreaSize;
			context_->SetVideoMode(videoMode);
			commands_->SetRenderTarget(*context_);

			// Update viewport
			LLGL::Viewport viewport;
			{
				viewport.width = static_cast<float>(videoMode.resolution.x);
				viewport.height = static_cast<float>(videoMode.resolution.y);
			}
			commands_->SetViewport(viewport);

			// Update scissor
			commands_->SetScissor({ 0, 0, videoMode.resolution.x, videoMode.resolution.y });

			// Update projection matrix
			projection_ = application_.PerspectiveProjection(viewport.width / viewport.height, 0.1f, 100.0f, Gs::Deg2Rad(45.0f));

			// Re-draw frame
			if (application_.IsLoadingDone())
				application_.OnDrawFrame();
		}

		void OnTimer(LLGL::Window& sender, unsigned int timerID) override
		{
			// Re-draw frame
			if (application_.IsLoadingDone())
				application_.OnDrawFrame();
		}

	private:

		Application&			application_;
		LLGL::RenderContext*    context_;
		LLGL::CommandBuffer*    commands_;
		Gs::Matrix4f&           projection_;

	};

	/* ----- Global helper functions ----- */

	static std::string GetSelectedRendererModule(int argc, char* argv[])
	{
		/* Select renderer module */
		std::string rendererModule;

		if (argc > 1)
		{
			/* Get renderer module name from command line argument */
			rendererModule = argv[1];
		}
		else
		{
			/* Find available modules */
			auto modules = LLGL::RenderSystem::FindModules();

			if (modules.empty())
			{
				/* No modules available -> throw error */
				throw std::runtime_error("no renderer modules available on target platform");
			}
			else// if (modules.size() == 1)
			{
				/* Use the only available module */
				rendererModule = modules.back();
			}
#if false
			else
			{
				/* Let user select a renderer */
				while (rendererModule.empty())
				{
					/* Print list of available modules */
					std::cout << "select renderer:" << std::endl;

					int i = 0;
					for (const auto& mod : modules)
						std::cout << " " << (++i) << ".) " << mod << std::endl;

					/* Wait for user input */
					std::size_t selection = 0;
					std::cin >> selection;
					--selection;

					if (selection < modules.size())
						rendererModule = modules[selection];
					else
						std::cerr << "invalid input" << std::endl;
				}
			}
#endif
		}

		/* Choose final renderer module */
		std::cout << "selected renderer: " << rendererModule << std::endl;

		return rendererModule;
	}

	std::string Application::rendererModule_;
	
	Application::Application(
		const std::wstring& title,
		const LLGL::Size&   resolution,
		unsigned int        multiSampling,
		bool                vsync,
		bool                debugger) :
		timer{ LLGL::Timer::Create() },
		context_(new Context())
	{
		context_->RegisterSubsystem<FileSystem>();

		Log& log = context_->RegisterSubsystem<Log>();
		log.Open("Unique.log");

		Graphics& graphics =  context_->RegisterSubsystem<Graphics>();

		graphics.Initialize(rendererModule_, resolution);

		// Set window title
		auto& window = static_cast<LLGL::Window&>(graphicsContext->GetSurface());

		auto rendererName = renderer->GetName();
		window.SetTitle(title + L" ( " + std::wstring(rendererName.begin(), rendererName.end()) + L" )");

		// Add input event listener to window
		input = std::make_shared<LLGL::Input>();
		window.AddEventListener(input);

		// Change window descriptor to allow resizing
		auto wndDesc = window.GetDesc();
		wndDesc.resizable = true;
		window.SetDesc(wndDesc);

		// Change window behavior
		auto behavior = window.GetBehavior();
		behavior.disableClearOnResize = true;
		behavior.moveAndResizeTimerID = 1;
		window.SetBehavior(behavior);

		// Add window resize listener
		window.AddEventListener(std::make_shared<ResizeEventHandler>(*this, graphicsContext, commands, projection));

		// Initialize default projection matrix
		projection = PerspectiveProjection(GetAspectRatio(), 0.1f, 100.0f, Gs::Deg2Rad(45.0f));

		// Show window
		window.Show();

		// Store information that loading is done
		loadingDone_ = true;
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		auto& window = static_cast<LLGL::Window&>(graphicsContext->GetSurface());
		while (window.ProcessEvents() && !input->KeyDown(LLGL::Key::Escape))
		{
		//	profilerObj_->ResetCounters();
			OnDrawFrame();
		}
	}

	void Application::SelectRendererModule(int argc, char* argv[])
	{
		rendererModule_ = GetSelectedRendererModule(argc, argv);
	}
	

}