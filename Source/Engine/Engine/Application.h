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
#include "../Graphics/Shader.h"
#include "../Resource/Image.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Texture.h"

namespace Unique
{


	struct VertexPositionTexCoord
	{
		Gs::Vector3f position;
		Gs::Vector2f texCoord;
	};

	class Application
	{
	public:
		Application(
			const std::wstring& title,
			const LLGL::Size&   resolution = { 800, 600 },
			unsigned int        multiSampling = 8,
			bool                vsync = true,
			bool                debugger = true);

		virtual ~Application();

		void Run();

		static void SelectRendererModule(int argc, char* argv[]);

	protected:

		// Used by the window resize handler
		bool IsLoadingDone() const
		{
			return loadingDone_;
		}

		Gs::Matrix4f PerspectiveProjection(float aspectRatio, float near, float far, float fov)
		{
			int flags = (IsOpenGL() ? Gs::ProjectionFlags::UnitCube : 0);
			return Gs::ProjectionMatrix4f::Perspective(aspectRatio, near, far, fov, flags).ToMatrix4();
		}

		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		LLGL::Texture* LoadTexture(const String& filename)
		{
			return Texture::Load(filename);
		}

		// Save texture image to a PNG file.
		bool SaveTexture(LLGL::Texture& texture, const String& filename, unsigned int mipLevel = 0)
		{
			return Texture::Save(texture, filename, mipLevel);
		}

		virtual void OnDrawFrame() = 0;

		static std::string				rendererModule_;
		bool                            loadingDone_ = false;
		std::shared_ptr<LLGL::Input>    input;
		UPtr<LLGL::Timer>				timer;
		Gs::Matrix4f                    projection;
		UPtr<Context>					context_;

		friend class ResizeEventHandler;

	};

	template <typename T>
	int RunApp(int argc, char* argv[])
	{
		try
		{
			/* Run tutorial */
			Application::SelectRendererModule(argc, argv);
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