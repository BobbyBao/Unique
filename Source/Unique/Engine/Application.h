/*
 * tutorial.h
 *
 * This file is part of the "LLGL" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_TUTORIAL_H
#define LLGL_TUTORIAL_H


#include <LLGL/LLGL.h>
#include <LLGL/Utility.h>
#include <Gauss/Gauss.h>

#include <vector>
#include <map>
#include <type_traits>

#include "../Core/Context.h"
#include "../Core/Thread.h"
#include "../Graphics/Technique.h"
#include "../Resource/Image.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Texture.h"

namespace Unique
{
	class Application : public Object
	{
	public:
		Application(const std::wstring& title, LLGL::Size resolution = { 800, 600 });

		virtual ~Application();

		void Run();

		static void Setup(int argc, char* argv[]);

	protected:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnPreRender();
		virtual void OnPostRender();

		static Vector<String>			argv_;
		static std::string				rendererModule_;
		std::wstring					title_;	
		LLGL::Size						resolution_;
		uint							multiSampling_ = 8;
		bool							vsync_ = true;
		bool							debugger = true;
		Window*							window_ = nullptr;
		bool                            loadingDone_ = false;
		std::shared_ptr<LLGL::Input>    input;
		UPtr<Context>					context_;

		friend class ResizeEventHandler;

	};

	template <typename T>
	int RunApp(int argc, char* argv[])
	{
		try
		{
			Application::Setup(argc, argv);
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


}


#define UNIQUE_IMPLEMENT_MAIN(CLASS)          \
		int main(int argc, char* argv[])            \
		{                                           \
			return Unique::RunApp<CLASS>(argc, argv);  \
		}



#endif