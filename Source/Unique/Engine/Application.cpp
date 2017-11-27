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
			Graphics& graphics = Subsystem<Graphics>();
			graphics.Resize(clientAreaSize);
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
		window_ = graphics.Initialize(rendererModule_, resolution_);
		
		auto rendererName = renderer->GetName();
		window_->SetTitle(title_ + L" ( " + std::wstring(rendererName.begin(), rendererName.end()) + L" )");

		// Add input event listener to window
		input = std::make_shared<LLGL::Input>();
		window_->AddEventListener(input);

		// Change window descriptor to allow resizing
		auto wndDesc = window_->GetDesc();
		wndDesc.resizable = true;
		window_->SetDesc(wndDesc);

		// Change window behavior
		auto behavior = window_->GetBehavior();
		behavior.disableClearOnResize = true;
		behavior.moveAndResizeTimerID = 1;
		window_->SetBehavior(behavior);

		// Add window resize listener
		window_->AddEventListener(std::make_shared<ResizeEventHandler>(*this));
		
		// Show window
		window_->Show();

		// Store information that loading is done
		loadingDone_ = true;
	}

	void Application::Terminate()
	{
	}

	void Application::Run()
	{
		Initialize();
	
		Renderer& renderer = Subsystem<Renderer>();

		context_->Run();

		while (window_->ProcessEvents() && !input->KeyDown(LLGL::Key::Escape))
		{
			renderer.Begin();

			OnPostRender();

			renderer.Render();

			renderer.End();
		}

		renderer.Stop();

		Terminate();

		context_->Stop();
	}

	void Application::OnPostRender()
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