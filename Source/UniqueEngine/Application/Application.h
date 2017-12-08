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

		void SetTitle(const std::wstring& title);
		void SetResolution(const IntVector2& res);

		void Run();
		
		static void Quit();
		static Vector<String>			argv_;
	protected:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnPreRender();
		virtual void OnPostRender();

		std::wstring					title_;
		IntVector2						resolution_;
		uint							multiSampling_ = 8;
		bool							vsync_ = true;
		bool							debugger = true;
		bool                            loadingDone_ = false;
		UPtr<Context>					context_;
		static bool						quit_;

		friend class ResizeEventHandler;

	};

	template <typename T>
	int RunApp(int argc, char* argv[])
	{
		try
		{
			Unique_Setup(argc, argv);
			auto app = UPtr<T>(new T());
			app->Run();
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
	
	UNIQUE_C_API void Unique_Setup(int argc, char* argv[]);
	UNIQUE_C_API int Unique_Start(const char* rendererModule, void* window);
	UNIQUE_C_API void Unique_Shutdown();
}


#define UNIQUE_IMPLEMENT_MAIN(CLASS)          \
		int main(int argc, char* argv[])            \
		{                                           \
			return Unique::RunApp<CLASS>(argc, argv);  \
		}
