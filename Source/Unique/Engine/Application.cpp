#include "Precompiled.h"
#include "Application.h"
#include "../IO/FileSystem.h"
#include <iostream>
#include <fstream>
#include "../Graphics/Graphics.h"
#include "../Graphics/Renderer.h"
#include "../Core/CoreEvents.h"
#include "../Core/WorkQueue.h"
#include "../Resource/ResourceCache.h"
#include "Graphics/Importers/ShaderImporter.h"
#include "Graphics/Importers/TextureImporter.h"
#include "Graphics/Importers/ModelImporter.h"

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
	
	Application::Application(const std::wstring& title, LLGL::Size resolution) :
		title_(title), 
		resolution_(resolution),
		context_(new Context())
	{
		context_->RegisterSubsystem<WorkQueue>();
		context_->RegisterSubsystem<Profiler>();
		context_->RegisterSubsystem<FileSystem>();

		Log& log = context_->RegisterSubsystem<Log>();
		log.Open("Unique.log");

		context_->RegisterSubsystem<ResourceCache>();
		context_->RegisterSubsystem<Graphics>();
		context_->RegisterSubsystem<Renderer>();

	}

	Application::~Application()
	{
	}

	void Application::Initialize()
	{
		auto& cache = Subsystem<ResourceCache>();
		cache.SetAutoReloadResources(true);
		cache.AddResourceDir("Assets");
		cache.AddResourceDir("CoreData");
		cache.AddResourceDir("Cache");

		cache.RegisterImporter(new ShaderImporter());
		cache.RegisterImporter(new TextureImporter());
		cache.RegisterImporter(new ModelImporter());

		Graphics& graphics = Subsystem<Graphics>();
		window_ = graphics.Initialize(rendererModule_, resolution_);
		
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
		window_->AddEventListener(std::make_shared<ResizeEventHandler>(*this));
		window_->Show();

		auto rendererName = graphics.GetRenderName();
		window_->SetTitle(title_ + L" ( " + std::wstring(rendererName.begin(), rendererName.end()) + L" )");
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
			
			OnPreRender();

			renderer.Render();

			OnPostRender();

			renderer.End();
		}

		renderer.Stop();

		Terminate();

		context_->Stop();
	}

	
	void Application::OnPreRender()
	{
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

		}

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