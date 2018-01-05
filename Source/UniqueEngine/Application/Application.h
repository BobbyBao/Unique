#pragma once

#include "Core/Context.h"
#include <iostream>

namespace Unique
{
	class Application : public Object
	{
	public:
		Application();
		virtual ~Application();

		void SetDeviceType(DeviceType deviceType) { deviceType_ = deviceType; }
		void SetTitle(const String& title);
		void SetResolution(const IntVector2& res);

		void Run();

		static void Setup(int argc, char* argv[]);
		static void Quit();

		static UPtr<Context>	context_;
		
	protected:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnPreRender();
		virtual void OnPostRender();

		static Vector<String> argv_;
		DeviceType	deviceType_;
		String		title_;
		IntVector2	resolution_;
		uint		multiSampling_ = 8;
		bool		vsync_ = true;
		bool		debugger = true;
		bool        loadingDone_ = false;
		static bool	quit_;

		friend class ResizeEventHandler;

	};

	template <typename T>
	int RunApp(int argc, char* argv[])
	{
		try
		{
			Application::context_.reset(new Context());
			Application::Setup(argc, argv);
			auto app = SPtr<T>(new T());
			app->Run();
			app = nullptr;
			Application::context_ = nullptr;
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			#ifdef _WIN32
			system("pause");
			#endif
		}
		return 0;
	}
	
	UNIQUE_C_API Application* Unique_Setup(int argc, char* argv[]);
	UNIQUE_C_API void Unique_Start(Application* app, DeviceType deviceType, void* window);
	UNIQUE_C_API void Unique_Shutdown();
}


#define UNIQUE_IMPLEMENT_MAIN(CLASS)          \
		int main(int argc, char* argv[])            \
		{                                           \
			return Unique::RunApp<CLASS>(argc, argv);  \
		}
