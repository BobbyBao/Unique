#pragma once
#include <iostream>

namespace Unique
{
	class Engine;

	class Application : public Object
	{
	public:
		Application(int argc = 0, char* argv[] = nullptr);
		virtual ~Application();

		void SetDeviceType(DeviceType deviceType);
		void Start();

		static void Quit();

	protected:
		virtual void Setup();
		virtual void Initialize();
		virtual void Terminate();

		SPtr<Engine> engine_;
		
	};

	template <typename T>
	int RunApp(int argc, char* argv[])
	{
		try
		{
			auto app = SPtr<T>(new T(argc, argv));
			app->Start();
			app = nullptr;
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
