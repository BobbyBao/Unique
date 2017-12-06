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
			graphics.Resize((const IntVector2&)clientAreaSize);
		}

	private:

		Application&			application_;
	};

	Vector<String> Application::argv_;
	std::string Application::rendererModule_ = "Direct3D11";
	bool Application::quit_ = false;
	Application::Application() :
		resolution_ { 800, 600 },
		context_(new Context())
	{
		context_->RegisterSubsystem<WorkQueue>();
		context_->RegisterSubsystem<Profiler>();
		context_->RegisterSubsystem<FileSystem>();

		Log& log = context_->RegisterSubsystem<Log>();
		log.Open("Unique.log");

		context_->RegisterSubsystem<Graphics>();

		context_->RegisterSubsystem<ResourceCache>();
	
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
		window_->SetTitle(title_);
		// Store information that loading is done
		loadingDone_ = true;
	}

	void Application::Terminate()
	{
	}

	void Application::Quit()
	{
		quit_ = true;
	}

	void Application::SetTitle(const std::wstring& title) 
	{
		title_ = title;
	}

	void Application::SetResolution(const IntVector2& res)
	{ 
		resolution_ = res; 
	}

	void Application::Run()
	{
		Initialize();
	
		Renderer& renderer = Subsystem<Renderer>();

		context_->Run();

		while (window_->ProcessEvents() && !quit_)
		{
			renderer.Begin();
			
			OnPreRender();

			renderer.Render();

			OnPostRender();

			renderer.End();
		}

		renderer.Stop();

		context_->Stop();

		Terminate();

		context_ = nullptr;
	}

	
	void Application::OnPreRender()
	{
	}

	void Application::OnPostRender()
	{
	}
	
	UNIQUE_C_API void Unique_Setup(int argc, char* argv[])
	{
		for (int i = 0; i < argc; i++)
		{
			Application::argv_.push_back(argv[i]);
		}

	}
	
	UNIQUE_C_API int Unique_Start(const char* rendererModule, LLGL::Surface* window)
	{
		try
		{
			Application::rendererModule_ = rendererModule;
			auto app = UPtr<Application>(new Application());
			app->Run();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
		return 0;
	}

	UNIQUE_C_API void Unique_Shutdown()
	{
		Application::Quit();
	}
}