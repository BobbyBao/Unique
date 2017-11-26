#include "Precompiled.h"
#include "Application.h"
#include "../IO/FileSystem.h"
#include <iostream>
#include <fstream>
#include "../Graphics/Graphics.h"
#include "../Graphics/Renderer.h"
#include "../Core/CoreEvents.h"

namespace Unique
{


	class ResizeEventHandler : public LLGL::Window::EventListener
	{
	public:
		ResizeEventHandler(Application& application)
			: application_{ application }
		{
		}

		void OnResize(LLGL::Window& sender, const LLGL::Size& clientAreaSize) override
		{
			auto videoMode = graphicsContext->GetVideoMode();

			// Update video mode
			videoMode.resolution = clientAreaSize;
			graphicsContext->SetVideoMode(videoMode);

			commands->SetRenderTarget(*graphicsContext);

			// Update viewport
			LLGL::Viewport viewport;
			{
				viewport.width = static_cast<float>(videoMode.resolution.x);
				viewport.height = static_cast<float>(videoMode.resolution.y);
			}
			commands->SetViewport(viewport);

			// Update scissor
			commands->SetScissor({ 0, 0, videoMode.resolution.x, videoMode.resolution.y });

		}
		
	private:

		Application&			application_;
	};


	Vector<String> Application::argv_;
	std::string Application::rendererModule_;
	
	Application::Application(
		const std::wstring& title,
		const LLGL::Size&   resolution,
		unsigned int        multiSampling,
		bool                vsync,
		bool                debugger) :
		context_(new Context()), title_(title), resolution_(resolution)
	{
		context_->RegisterSubsystem<FileSystem>();

		Log& log = context_->RegisterSubsystem<Log>();
		log.Open("Unique.log");

		context_->RegisterSubsystem<Graphics>();
		context_->RegisterSubsystem<Renderer>();

	}

	Application::~Application()
	{
	}

	void Application::Initialize()
	{
		Graphics& graphics = Subsystem<Graphics>();
		graphics.Initialize(rendererModule_, resolution_);

		// Set window title
		auto& window = static_cast<LLGL::Window&>(graphicsContext->GetSurface());

		auto rendererName = renderer->GetName();
		window.SetTitle(title_ + L" ( " + std::wstring(rendererName.begin(), rendererName.end()) + L" )");

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
		window.AddEventListener(std::make_shared<ResizeEventHandler>(*this));
		
		// Show window
		window.Show();

		// Store information that loading is done
		loadingDone_ = true;
	}

	void Application::Terminate()
	{
	}

	void Application::Run()
	{
		Initialize();
	
		Graphics& graphics = Subsystem<Graphics>();

		auto& window = static_cast<LLGL::Window&>(graphicsContext->GetSurface());

		context_->Run();

		while (window.ProcessEvents() && !input->KeyDown(LLGL::Key::Escape))
		{
		//	profilerObj_->ResetCounters();

			OnDrawFrame();

			graphics.RenderFrame();
		}

		graphics.RenderFrame();

		Terminate();

		context_->Stop();
	}

	void Application::OnDrawFrame()
	{
	}

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
				rendererModule = modules.front();
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

	void Application::Setup(int argc, char* argv[])
	{
		for (int i = 0; i < argc; i++)
		{
			argv_.push_back(argv[i]);
		}

		rendererModule_ = GetSelectedRendererModule(argc, argv);
	}
	

}