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

namespace Unique
{
	struct VertexPositionNormal
	{
		Gs::Vector3f position;
		Gs::Vector3f normal;
	};

	struct VertexPositionTexCoord
	{
		Gs::Vector3f position;
		Gs::Vector2f texCoord;
	};

	struct ShaderStage
	{
		ShaderStage(
			LLGL::ShaderType type, const std::string& filename) :
			type{ type },
			filename{ filename }
		{
		}

		ShaderStage(
			LLGL::ShaderType type, const std::string& filename, const std::string& entryPoint, const std::string& target) :
			type{ type },
			filename{ filename },
			entryPoint{ entryPoint },
			target{ target }
		{
		}

		LLGL::ShaderType    type;
		std::string         filename;
		std::string         entryPoint;
		std::string         target;
	};


	class Application
	{
	public:

		virtual ~Application()
		{
		}

		void Run()
		{
			auto& window = static_cast<LLGL::Window&>(context->GetSurface());
			while (window.ProcessEvents() && !input->KeyDown(LLGL::Key::Escape))
			{
				profilerObj_->ResetCounters();
				OnDrawFrame();
			}
		}

		static void SelectRendererModule(int argc, char* argv[]);
		static byte* LoadImage(const char* filename, int *x, int *y, int *comp, int req_comp);
		static void FreeImage(void *retval_from_stbi_load);
	protected:

		struct ShaderProgramRecall
		{
			std::vector<ShaderStage>   shaderDescs;
			std::vector<LLGL::Shader*>              shaders;
			LLGL::VertexFormat                      vertexFormat;
			LLGL::StreamOutputFormat                streamOutputFormat;
		};

		Application(
			const std::wstring& title,
			const LLGL::Size&   resolution = { 800, 600 },
			unsigned int        multiSampling = 8,
			bool                vsync = true,
			bool                debugger = true);

		virtual void OnDrawFrame() = 0;

		LLGL::ShaderProgram* LoadShaderProgram(
			const std::vector<ShaderStage>& shaderDescs,
			const LLGL::VertexFormat& vertexFormat = {},
			const LLGL::StreamOutputFormat& streamOutputFormat = {});

		// Reloads the specified shader program from the previously specified shader source files.
		bool ReloadShaderProgram(LLGL::ShaderProgram* shaderProgram);

		// Load standard shader program (with vertex- and fragment shaders)
		LLGL::ShaderProgram* LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat);

		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		LLGL::Texture* LoadTexture(const std::string& filename)
		{
			return LoadTextureWithRenderer(*renderer, filename);
		}


		UPtr<LLGL::RenderingProfiler>    profilerObj_;
		UPtr<LLGL::RenderingDebugger>    debuggerObj_;

		std::map< LLGL::ShaderProgram*,
			ShaderProgramRecall >             shaderPrograms_;

		bool                                        loadingDone_ = false;

		static std::string                          rendererModule_;

		const LLGL::ColorRGBAf                      defaultClearColor{ 0.1f, 0.1f, 0.4f };

		// Render system
		UPtr<LLGL::RenderSystem>         renderer;

		// Main render context
		LLGL::RenderContext*                        context = nullptr;

		// Main command buffer
		LLGL::CommandBuffer*                        commands = nullptr;

		std::shared_ptr<LLGL::Input>                input;

		UPtr<LLGL::Timer>                timer;
		const LLGL::RenderingProfiler&              profiler;

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

		// Loads the vertices with position and normal from the specified Wavefront OBJ model file.
		static std::vector<VertexPositionNormal> LoadObjModel(const std::string& filename);
		
		// Generates eight vertices for a unit cube.
		static std::vector<Gs::Vector3f> GenerateCubeVertices()
		{
			return
			{
				{ -1, -1, -1 }, { -1,  1, -1 }, {  1,  1, -1 }, {  1, -1, -1 },
				{ -1, -1,  1 }, { -1,  1,  1 }, {  1,  1,  1 }, {  1, -1,  1 },
			};
		}

		// Generates 36 indices for a unit cube of 8 vertices
		// (36 = 3 indices per triangle * 2 triangles per cube face * 6 faces).
		static std::vector<std::uint32_t> GenerateCubeTriangleIndices()
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
		static std::vector<std::uint32_t> GenerateCubeQuadlIndices()
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
		static std::vector<VertexPositionTexCoord> GenerateTexturedCubeVertices()
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
		static std::vector<std::uint32_t> GenerateTexturedCubeTriangleIndices()
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

		template <typename VertexType>
		LLGL::Buffer* CreateVertexBuffer(const std::vector<VertexType>& vertices, const LLGL::VertexFormat& vertexFormat)
		{
			return renderer->CreateBuffer(
				LLGL::VertexBufferDesc(static_cast<unsigned int>(vertices.size() * sizeof(VertexType)), vertexFormat),
				vertices.data()
			);
		}

		template <typename IndexType>
		LLGL::Buffer* CreateIndexBuffer(const std::vector<IndexType>& indices, const LLGL::IndexFormat& indexFormat)
		{
			return renderer->CreateBuffer(
				LLGL::IndexBufferDesc(static_cast<unsigned int>(indices.size() * sizeof(IndexType)), indexFormat),
				indices.data()
			);
		}

		template <typename Buffer>
		LLGL::Buffer* CreateConstantBuffer(const Buffer& buffer)
		{
			static_assert(!std::is_pointer<Buffer>::value, "buffer type must not be a pointer");
			return renderer->CreateBuffer(
				LLGL::ConstantBufferDesc(sizeof(buffer)),
				&buffer
			);
		}

		template <typename T>
		void UpdateBuffer(LLGL::Buffer* buffer, const T& data)
		{
			GS_ASSERT(buffer != nullptr);
			renderer->WriteBuffer(*buffer, &data, sizeof(data), 0);
		}

		// Returns the aspect ratio of the render context resolution (X:Y).
		float GetAspectRatio() const
		{
			auto resolution = context->GetVideoMode().resolution.Cast<float>();
			return (resolution.x / resolution.y);
		}

		// Returns ture if OpenGL is used as rendering API.
		bool IsOpenGL() const
		{
			return (renderer->GetRendererID() == LLGL::RendererID::OpenGL);
		}

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
			auto tutorial = UPtr<T>(new T());
			tutorial->Run();
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