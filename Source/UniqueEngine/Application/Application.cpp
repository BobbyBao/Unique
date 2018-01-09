#include "UniquePCH.h"
#include "Application.h"
#include "Application/Engine.h"
#include "../Input/Input.h"


namespace Unique
{
	Application::Application(int argc, char* argv[])
	{
		Engine::context_ = new Context();
		Engine::Setup(argc, argv);
		engine_ = new Engine();
	}

	Application::~Application()
	{
		Engine::context_ = nullptr;
	}

	void Application::SetDeviceType(DeviceType deviceType)
	{
		GetSubsystem<Engine>().SetDeviceType(deviceType);
	}

	void Application::Terminate()
	{
	}

	void Application::Quit()
	{
		Engine::quit_ = true;
	}

	void Application::Setup()
	{
	}

	void Application::Initialize()
	{
	}

	void Application::Start()
	{
		Setup();

		engine_->Initialize();

		Initialize();

		engine_->Start();

		Terminate();
	}

	UNIQUE_C_API Application* Unique_Setup(int argc, char* argv[])
	{
		Application* app = new Application(argc, argv);
		app->AddRef();
		return app;
	}
	
	UNIQUE_C_API void Unique_Start(Application* app, DeviceType deviceType, void* window)
	{
		try
		{	
			app->SetDeviceType(deviceType);
			app->Start();
			app->ReleaseRef();
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