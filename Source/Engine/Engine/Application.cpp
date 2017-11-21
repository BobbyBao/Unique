#include "Precompiled.h"
#include "Application.h"

#include <iostream>
#include <fstream>
#include "../Graphics/Graphics.h"

namespace Unique
{


	class ResizeEventHandler : public LLGL::Window::EventListener
	{
	public:
		ResizeEventHandler(Application& application,
			LLGL::RenderContext* context,
			LLGL::CommandBuffer* commands,
			Gs::Matrix4f& projection) :
			application_{ application },
			context_{ context },
			commands_{ commands },
			projection_{ projection }
		{
		}

		void OnResize(LLGL::Window& sender, const LLGL::Size& clientAreaSize) override
		{
			auto videoMode = context_->GetVideoMode();

			// Update video mode
			videoMode.resolution = clientAreaSize;
			context_->SetVideoMode(videoMode);
			commands_->SetRenderTarget(*context_);

			// Update viewport
			LLGL::Viewport viewport;
			{
				viewport.width = static_cast<float>(videoMode.resolution.x);
				viewport.height = static_cast<float>(videoMode.resolution.y);
			}
			commands_->SetViewport(viewport);

			// Update scissor
			commands_->SetScissor({ 0, 0, videoMode.resolution.x, videoMode.resolution.y });

			// Update projection matrix
			projection_ = application_.PerspectiveProjection(viewport.width / viewport.height, 0.1f, 100.0f, Gs::Deg2Rad(45.0f));

			// Re-draw frame
			if (application_.IsLoadingDone())
				application_.OnDrawFrame();
		}

		void OnTimer(LLGL::Window& sender, unsigned int timerID) override
		{
			// Re-draw frame
			if (application_.IsLoadingDone())
				application_.OnDrawFrame();
		}

	private:

		Application&			application_;
		LLGL::RenderContext*    context_;
		LLGL::CommandBuffer*    commands_;
		Gs::Matrix4f&           projection_;

	};

	/* ----- Global helper functions ----- */

	static std::string GetSelectedRendererModule(int argc, char* argv[])
	{
		/* Select renderer module */
		std::string rendererModule;

		if (argc > 1)
		{
			/* Get renderer module name from command line argument */
			rendererModule = argv[1];
		}
		else
		{
			/* Find available modules */
			auto modules = LLGL::RenderSystem::FindModules();

			if (modules.empty())
			{
				/* No modules available -> throw error */
				throw std::runtime_error("no renderer modules available on target platform");
			}
			else// if (modules.size() == 1)
			{
				/* Use the only available module */
				rendererModule = modules.front();
			}
#if false
			else
			{
				/* Let user select a renderer */
				while (rendererModule.empty())
				{
					/* Print list of available modules */
					std::cout << "select renderer:" << std::endl;

					int i = 0;
					for (const auto& mod : modules)
						std::cout << " " << (++i) << ".) " << mod << std::endl;

					/* Wait for user input */
					std::size_t selection = 0;
					std::cin >> selection;
					--selection;

					if (selection < modules.size())
						rendererModule = modules[selection];
					else
						std::cerr << "invalid input" << std::endl;
				}
			}
#endif
		}

		/* Choose final renderer module */
		std::cout << "selected renderer: " << rendererModule << std::endl;

		return rendererModule;
	}

	static std::string ReadFileContent(const std::string& filename)
	{
		// Read shader file
		std::ifstream file(filename);

		if (!file.good())
			throw std::runtime_error("failed to open file: \"" + filename + "\"");

		return std::string(
			(std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>())
		);
	}


	std::string Application::rendererModule_;
	
	Application::Application(
		const std::wstring& title,
		const LLGL::Size&   resolution,
		unsigned int        multiSampling,
		bool                vsync,
		bool                debugger) :
		timer{ LLGL::Timer::Create() },
		context_(new Context())
	{
		Graphics& graphics =  context_->RegisterSubsystem<Graphics>();
		graphics.Initialize(rendererModule_, resolution);


		// Set window title
		auto& window = static_cast<LLGL::Window&>(graphics.GetSurface());

		auto rendererName = graphics.GetRenderSystem().GetName();
		window.SetTitle(title + L" ( " + std::wstring(rendererName.begin(), rendererName.end()) + L" )");

		// Add input event listener to window
		input = std::make_shared<LLGL::Input>();
		window.AddEventListener(input);

		// Change window descriptor to allow resizing
		auto wndDesc = window.GetDesc();
		wndDesc.resizable = true;
		window.SetDesc(wndDesc);

		// Change window behavior
		auto behavior = window.GetBehavior();
		behavior.disableClearOnResize = true;
		behavior.moveAndResizeTimerID = 1;
		window.SetBehavior(behavior);

		// Add window resize listener
		window.AddEventListener(std::make_shared<ResizeEventHandler>(*this, graphics.GetRenderContext(),
			graphics.commands, projection));

		// Initialize default projection matrix
		projection = PerspectiveProjection(GetAspectRatio(), 0.1f, 100.0f, Gs::Deg2Rad(45.0f));

		// Show window
		window.Show();

		// Store information that loading is done
		loadingDone_ = true;
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		auto& window = static_cast<LLGL::Window&>(context->GetSurface());
		while (window.ProcessEvents() && !input->KeyDown(LLGL::Key::Escape))
		{
		//	profilerObj_->ResetCounters();
			OnDrawFrame();
		}
	}

	void Application::SelectRendererModule(int argc, char* argv[])
	{
		rendererModule_ = GetSelectedRendererModule(argc, argv);
	}
	
	// Load image from file, create texture, upload image into texture, and generate MIP-maps.
	LLGL::Texture* Application::LoadTextureWithRenderer(LLGL::RenderSystem& renderSys, const std::string& filename)
	{
		// Load image data from file (using STBI library, see https://github.com/nothings/stb)
		int width = 0, height = 0, components = 0;

		unsigned char* imageBuffer = Image::LoadImage(filename.c_str(), &width, &height, &components, 4);
		if (!imageBuffer)
			throw std::runtime_error("failed to load texture from file: \"" + filename + "\"");

		// Initialize image descriptor to upload image data onto hardware texture
		LLGL::ImageDescriptor imageDesc;
		{
			// Set image buffer color format
			imageDesc.format = LLGL::ImageFormat::RGBA;

			// Set image buffer data type (unsigned char = 8-bit unsigned integer)
			imageDesc.dataType = LLGL::DataType::UInt8;

			// Set image buffer source for texture initial data
			imageDesc.buffer = imageBuffer;
		}

		// Create texture and upload image data onto hardware texture
		auto tex = renderSys.CreateTexture(
			LLGL::Texture2DDesc(LLGL::TextureFormat::RGBA, width, height), &imageDesc
		);

		// Generate all MIP-maps (MIP = "Multum in Parvo", or "a multitude in a small space")
		// see https://developer.valvesoftware.com/wiki/MIP_Mapping
		// see http://whatis.techtarget.com/definition/MIP-map
		renderSys.GenerateMips(*tex);

		// Release image data
		Image::FreeImage(imageBuffer);

		// Show info
		std::cout << "loaded texture: " << filename << std::endl;

		return tex;
	}
	
	bool Application::SaveTextureWithRenderer(LLGL::RenderSystem& renderSys, LLGL::Texture& texture, const std::string& filename, unsigned int mipLevel)
	{/*
		// Get texture dimension
		auto texSize = texture.QueryMipLevelSize(0).Cast<int>();

		// Read texture image data
		std::vector<LLGL::ColorRGBAub> imageBuffer(texSize.x*texSize.y);
		renderSys.ReadTexture(texture, mipLevel, LLGL::ImageFormat::RGBA, LLGL::DataType::UInt8, imageBuffer.data());

		// Save image data to file (using STBI library, see https://github.com/nothings/stb)
		if (!stbi_write_png(filename.c_str(), texSize.x, texSize.y, 4, imageBuffer.data(), texSize.x * 4))
		{
			std::cerr << "failed to write texture to file: \"" + filename + "\"" << std::endl;
			return false;
		}

		// Show info
		std::cout << "saved texture: " << filename << std::endl;
		*/
		return true;
	}

	LLGL::ShaderProgram* Application::LoadShaderProgram(
		const std::vector<ShaderStage>& shaderDescs,
		const LLGL::VertexFormat& vertexFormat,
		const LLGL::StreamOutputFormat& streamOutputFormat)
	{
		// Create shader program
		LLGL::ShaderProgram* shaderProgram = renderer->CreateShaderProgram();

		ShaderProgramRecall recall;

		recall.shaderDescs = shaderDescs;

		for (const auto& desc : shaderDescs)
		{
			// Read shader file
			auto shaderCode = ReadFileContent(desc.filename);

			// Create shader
			auto shader = renderer->CreateShader(desc.type);

			// Compile shader
			LLGL::ShaderDescriptor shaderDesc{ desc.entryPoint, desc.target, LLGL::ShaderCompileFlags::Debug };
			shaderDesc.streamOutput.format = streamOutputFormat;

			shader->Compile(shaderCode, shaderDesc);

			// Print info log (warnings and errors)
			std::string log = shader->QueryInfoLog();
			if (!log.empty())
				std::cerr << log << std::endl;

			// Attach vertex- and fragment shader to the shader program
			shaderProgram->AttachShader(*shader);

			// Store shader in recall
			recall.shaders.push_back(shader);
		}

		// Bind vertex attribute layout (this is not required for a compute shader program)
		if (!vertexFormat.attributes.empty())
			shaderProgram->BuildInputLayout(vertexFormat);

		// Link shader program and check for errors
		if (!shaderProgram->LinkShaders())
			throw std::runtime_error(shaderProgram->QueryInfoLog());

		// Store information in call
		recall.vertexFormat = vertexFormat;
		recall.streamOutputFormat = streamOutputFormat;
		shaderPrograms_[shaderProgram] = recall;

		return shaderProgram;
	}

	// Reloads the specified shader program from the previously specified shader source files.
	bool Application::ReloadShaderProgram(LLGL::ShaderProgram* shaderProgram)
	{
		std::cout << "reload shader program" << std::endl;

		// Find shader program in the recall map
		auto it = shaderPrograms_.find(shaderProgram);
		if (it == shaderPrograms_.end())
			return false;

		auto& recall = it->second;
		std::vector<LLGL::Shader*> shaders;

		// Detach previous shaders
		shaderProgram->DetachAll();

		try
		{
			// Recompile all shaders
			for (const auto& desc : recall.shaderDescs)
			{
				// Read shader file
				auto shaderCode = ReadFileContent(desc.filename);

				// Create shader
				auto shader = Subsystem<Graphics>().GetRenderSystem().CreateShader(desc.type);

				// Compile shader
				LLGL::ShaderDescriptor shaderDesc(desc.entryPoint, desc.target, LLGL::ShaderCompileFlags::Debug);
				shaderDesc.streamOutput.format = recall.streamOutputFormat;

				shader->Compile(shaderCode, shaderDesc);

				// Print info log (warnings and errors)
				std::string log = shader->QueryInfoLog();
				if (!log.empty())
					std::cerr << log << std::endl;

				// Attach vertex- and fragment shader to the shader program
				shaderProgram->AttachShader(*shader);

				// Store new shader
				shaders.push_back(shader);
			}

			// Bind vertex attribute layout (this is not required for a compute shader program)
			if (!recall.vertexFormat.attributes.empty())
				shaderProgram->BuildInputLayout(recall.vertexFormat);

			// Link shader program and check for errors
			if (!shaderProgram->LinkShaders())
				throw std::runtime_error(shaderProgram->QueryInfoLog());
		}
		catch (const std::exception& err)
		{
			// Print error message
			std::cerr << err.what() << std::endl;

			// Attach all previous shaders again
			for (auto shader : recall.shaders)
				shaderProgram->AttachShader(*shader);

			// Bind vertex attribute layout (this is not required for a compute shader program)
			if (!recall.vertexFormat.attributes.empty())
				shaderProgram->BuildInputLayout(recall.vertexFormat);

			// Link shader program and check for errors
			if (!shaderProgram->LinkShaders())
				throw std::runtime_error(shaderProgram->QueryInfoLog());

			return false;
		}

		// Delete all previous shaders
		for (auto shader : recall.shaders)
			renderer->Release(*shader);

		// Store new shaders in recall
		recall.shaders = std::move(shaders);

		return true;
	}

	// Load standard shader program (with vertex- and fragment shaders)
	LLGL::ShaderProgram* Application::LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat)
	{
		// Load shader program
		if (renderer->GetRenderingCaps().shadingLanguage >= LLGL::ShadingLanguage::HLSL_2_0)
		{
			return LoadShaderProgram(
			{
				{ LLGL::ShaderType::Vertex, "Assets/shader.hlsl", "VS", "vs_5_0" },
				{ LLGL::ShaderType::Fragment, "Assets/shader.hlsl", "PS", "ps_5_0" }
			},
				vertexFormat
			);
		}
		else
		{
			return LoadShaderProgram(
			{
				{ LLGL::ShaderType::Vertex, "Assets/vertex.glsl" },
				{ LLGL::ShaderType::Fragment, "Assets/fragment.glsl" }
			},
				vertexFormat
			);
		}
	}
}