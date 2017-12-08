#include "UniquePCH.h"
#include "Application.h"
#include <SDL/SDL.h>
#include "Input.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Renderer.h"
#include "Core/CoreEvents.h"
#include "Core/WorkQueue.h"
#include "Graphics/Importers/ShaderImporter.h"
#include "Graphics/Importers/TextureImporter.h"
#include "Graphics/Importers/ModelImporter.h"

namespace Unique
{

	Vector<String> Application::argv_;
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
		context_->RegisterSubsystem<Input>();

	}

	Application::~Application()
	{
	}

	void Application::Initialize()
	{
		/* Enable standard application logging */
		SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

		/* Initialize SDL */
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
			return;
		}

		auto& cache = Subsystem<ResourceCache>();
		cache.SetAutoReloadResources(true);
		cache.AddResourceDir("Assets");
		cache.AddResourceDir("CoreData");
		cache.AddResourceDir("Cache");

		cache.RegisterImporter(new ShaderImporter());
		cache.RegisterImporter(new TextureImporter());
		cache.RegisterImporter(new ModelImporter());

		Graphics& graphics = Subsystem<Graphics>();
		graphics.Initialize(resolution_);
		
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
		Input& input = Subsystem<Input>();

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
		if (done) {
			emscripten_cancel_main_loop();
		}
#endif
		renderer.Stop();

		context_->Stop();

		Terminate();

		SDL_Quit();
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
	
	UNIQUE_C_API int Unique_Start(const char* rendererModule, void* window)
	{
		try
		{
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