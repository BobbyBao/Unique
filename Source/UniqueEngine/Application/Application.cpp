#include "UniquePCH.h"
#include "Application.h"
#include "../Input/Input.h"
#include "Core/CoreEvents.h"
#include "Core/WorkQueue.h"
#include "Graphics/Importers/ShaderImporter.h"
#include "Resource/Image.h"
#include "Graphics/Importers/TextureImporter.h"
#include "Graphics/Importers/ModelImporter.h"
#include "ImGUI/ImGUI.h"

namespace Unique
{
	UPtr<Context> Application::context_;
	Vector<String> Application::argv_;
	bool Application::quit_ = false;

	Application::Application():
		resolution_(800, 600),
		deviceType_(DeviceType::D3D11)
	{
		context_->RegisterSubsystem<WorkQueue>();
		context_->RegisterSubsystem<Profiler>();
		context_->RegisterSubsystem<FileSystem>();
		context_->RegisterSubsystem<Log>("Unique.log");
		context_->RegisterSubsystem<Graphics>();
		context_->RegisterSubsystem<ResourceCache>();
		context_->RegisterSubsystem<Renderer>();
		context_->RegisterSubsystem<Input>();
		context_->RegisterSubsystem<GUISystem>();

	}

	Application::~Application()
	{
	}

	void Application::Setup(int argc, char* argv[])
	{
		for (int i = 0; i < argc; i++)
		{
			argv_.push_back(argv[i]);
		}
	}

	void Application::Initialize()
	{
		auto& cache = GetSubsystem<ResourceCache>();
		auto& graphics = GetSubsystem<Graphics>();

		cache.SetAutoReloadResources(true);
		cache.AddResourceDir("Assets");
		cache.AddResourceDir("CoreData");
		cache.AddResourceDir("Cache");

		cache.RegisterImporter(new ShaderImporter());
		cache.RegisterImporter(new ImageImporter());
		cache.RegisterImporter(new TextureImporter());
		cache.RegisterImporter(new ModelImporter());
		cache.RegisterImporter(new AnimationImporter());

		graphics.Initialize(resolution_, deviceType_);
		
		loadingDone_ = true;
	}

	void Application::Terminate()
	{
	}

	void Application::Quit()
	{
		quit_ = true;
	}

	void Application::SetTitle(const String& title) 
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
	
		auto& renderer = GetSubsystem<Renderer>();
		auto& input = GetSubsystem<Input>();

		context_->Run();

		while (input.ProcessEvents() && !quit_)
		{
			renderer.Begin();
			
			OnPreRender();

			renderer.Render();

			OnPostRender();

			renderer.End();
		}

#ifdef __EMSCRIPTEN__
		
		emscripten_cancel_main_loop();
		
#endif
		renderer.Stop();

		context_->Stop();

		Terminate();

	}

	void Application::OnPreRender()
	{
	}

	void Application::OnPostRender()
	{
	}

	UNIQUE_C_API Application* Unique_Setup(int argc, char* argv[])
	{
		Application::context_.reset(new Context());
		Application::Setup(argc, argv);
		Application* app = new Application();

		return app;
	}
	
	UNIQUE_C_API void Unique_Start(Application* app, DeviceType deviceType, void* window)
	{
		try
		{	
			app->SetDeviceType(deviceType);
			app->Run();
			app->ReleaseRef();
			Application::context_ = nullptr;
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}

	UNIQUE_C_API void Unique_Shutdown()
	{
		Application::Quit();
	}
}