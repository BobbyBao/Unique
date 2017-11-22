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


		virtual void OnDrawFrame() = 0;

		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		LLGL::Texture* LoadTexture(const std::string& filename)
		{
			return LoadTextureWithRenderer(*renderer, filename);
		}

		static std::string                          rendererModule_;
		
		bool                                        loadingDone_ = false;

		std::shared_ptr<LLGL::Input>                input;

		UPtr<LLGL::Timer>                timer;

		Gs::Matrix4f                                projection;

		UPtr<Context> context_;


		friend class ResizeEventHandler;

	public:
		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		static LLGL::Texture* LoadTextureWithRenderer(LLGL::RenderSystem& renderSys, const std::string& filename);
		// Save texture image to a PNG file.
		static bool SaveTextureWithRenderer(LLGL::RenderSystem& renderSys, LLGL::Texture& texture, const std::string& filename, unsigned int mipLevel = 0);

	protected:

		// Save texture image to a PNG file.
		bool SaveTexture(LLGL::Texture& texture, const std::string& filename, unsigned int mipLevel = 0)
		{
			return SaveTextureWithRenderer(*renderer, texture, filename, mipLevel);
		}

	public:

		// Generates eight vertices for a unit cube.
		static Vector<Gs::Vector3f> GenerateCubeVertices()
		{
			return
			{
				{ -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 }, {  1, -1, -1 },
				{ -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 }, {  1, -1,  1 },
			};
		}

		// Generates 36 indices for a unit cube of 8 vertices
		// (36 = 3 indices per triangle * 2 triangles per cube face * 6 faces).
		static Vector<std::uint32_t> GenerateCubeTriangleIndices()
		{
			return
			{
				0, 1, 2, 0, 2, 3, // front
				3, 2, 6, 3, 6, 7, // right
				4, 5, 1, 4, 1, 0, // left
				1, 5, 6, 1, 6, 2, // top
				4, 0, 3, 4, 3, 7, // bottom
				7, 6, 5, 7, 5, 4, // back
			};
		}

		// Generates 24 indices for a unit cube of 8 vertices.
		// (24 = 4 indices per quad * 1 quad per cube face * 6 faces)
		static Vector<std::uint32_t> GenerateCubeQuadlIndices()
		{
			return
			{
				0, 1, 3, 2, // front
				3, 2, 7, 6, // right
				4, 5, 0, 1, // left
				1, 5, 2, 6, // top
				4, 0, 7, 3, // bottom
				7, 6, 4, 5, // back
			};
		}

		// Generates 24 vertices for a unit cube with texture coordinates.
		static Vector<VertexPositionTexCoord> GenerateTexturedCubeVertices()
		{
			return
			{
				{ { -1, -1, -1 }, { 0, 1 } }, { { -1,  1, -1 }, { 0, 0 } }, { {  1,  1, -1 }, { 1, 0 } }, { {  1, -1, -1 }, { 1, 1 } }, // front
				{ {  1, -1, -1 }, { 0, 1 } }, { {  1,  1, -1 }, { 0, 0 } }, { {  1,  1,  1 }, { 1, 0 } }, { {  1, -1,  1 }, { 1, 1 } }, // right
				{ { -1, -1,  1 }, { 0, 1 } }, { { -1,  1,  1 }, { 0, 0 } }, { { -1,  1, -1 }, { 1, 0 } }, { { -1, -1, -1 }, { 1, 1 } }, // left
				{ { -1,  1, -1 }, { 0, 1 } }, { { -1,  1,  1 }, { 0, 0 } }, { {  1,  1,  1 }, { 1, 0 } }, { {  1,  1, -1 }, { 1, 1 } }, // top
				{ { -1, -1,  1 }, { 0, 1 } }, { { -1, -1, -1 }, { 0, 0 } }, { {  1, -1, -1 }, { 1, 0 } }, { {  1, -1,  1 }, { 1, 1 } }, // bottom
				{ {  1, -1,  1 }, { 0, 1 } }, { {  1,  1,  1 }, { 0, 0 } }, { { -1,  1,  1 }, { 1, 0 } }, { { -1, -1,  1 }, { 1, 1 } }, // back
			};
		}

		// Generates 36 indices for a unit cube of 24 vertices
		static Vector<std::uint32_t> GenerateTexturedCubeTriangleIndices()
		{
			return
			{
				 0,  1,  2,  0,  2,  3, // front
				 4,  5,  6,  4,  6,  7, // right
				 8,  9, 10,  8, 10, 11, // left
				12, 13, 14, 12, 14, 15, // top
				16, 17, 18, 16, 18, 19, // bottom
				20, 21, 22, 20, 22, 23, // back
			};
		}

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