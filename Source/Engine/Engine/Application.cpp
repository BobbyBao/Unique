#include "Precompiled.h"
#include "Application.h"

#include <iostream>
#include <fstream>

namespace Unique
{
	class Debugger : public LLGL::RenderingDebugger
	{
		void OnError(LLGL::ErrorType type, Message& message) override
		{
			std::cerr << "ERROR: " << message.GetSource() << ": " << message.GetText() << std::endl;
			message.Block();
		}

		void OnWarning(LLGL::WarningType type, Message& message) override
		{
			std::cerr << "WARNING: " << message.GetSource() << ": " << message.GetText() << std::endl;
			message.Block();
		}

	};

	class ResizeEventHandler : public LLGL::Window::EventListener
	{
	public:
		ResizeEventHandler(
			Application& tutorial,
			LLGL::RenderContext* context,
			LLGL::CommandBuffer* commands,
			Gs::Matrix4f& projection) :
			tutorial_{ tutorial },
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
			projection_ = tutorial_.PerspectiveProjection(viewport.width / viewport.height, 0.1f, 100.0f, Gs::Deg2Rad(45.0f));

			// Re-draw frame
			if (tutorial_.IsLoadingDone())
				tutorial_.OnDrawFrame();
		}

		void OnTimer(LLGL::Window& sender, unsigned int timerID) override
		{
			// Re-draw frame
			if (tutorial_.IsLoadingDone())
				tutorial_.OnDrawFrame();
		}

	private:

		Application&               tutorial_;
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

	void Application::SelectRendererModule(int argc, char* argv[])
	{
		rendererModule_ = GetSelectedRendererModule(argc, argv);
	}
	
	// Loads the vertices with position and normal from the specified Wavefront OBJ model file.
	std::vector<VertexPositionNormal> Application::LoadObjModel(const std::string& filename)
	{
		// Read obj file
		std::ifstream file(filename);
		if (!file.good())
			throw std::runtime_error("failed to load model from file: \"" + filename + "\"");

		std::vector<Gs::Vector3f> coords, normals;
		std::vector<VertexPositionNormal> vertices;

		while (!file.eof())
		{
			// Read each line
			std::string line;
			std::getline(file, line);

			std::stringstream s;
			s << line;

			// Parse line
			std::string mode;
			s >> mode;

			if (mode == "v")
			{
				Gs::Vector3f v;
				s >> v.x;
				s >> v.y;
				s >> v.z;
				coords.push_back(v);
			}
			else if (mode == "vn")
			{
				Gs::Vector3f n;
				s >> n.x;
				s >> n.y;
				s >> n.z;
				normals.push_back(n);
			}
			else if (mode == "f")
			{
				unsigned int v = 0, vn = 0;

				for (int i = 0; i < 3; ++i)
				{
					s >> v;
					s.ignore(2);
					s >> vn;
					vertices.push_back({ coords[v - 1], normals[vn - 1] });
				}
			}
		}

		return vertices;
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

	Application::Application(
		const std::wstring& title,
		const LLGL::Size&   resolution,
		unsigned int        multiSampling,
		bool                vsync,
		bool                debugger) :
		profilerObj_{ new LLGL::RenderingProfiler() },
		debuggerObj_{ new Debugger() },
		timer{ LLGL::Timer::Create() },
		profiler{ *profilerObj_ },

		context_(new Context())
	{
		// Create render system
		renderer = LLGL::RenderSystem::Load(
			rendererModule_,
			(debugger ? profilerObj_.get() : nullptr),
			(debugger ? debuggerObj_.get() : nullptr)
		);

		// Create render context
		LLGL::RenderContextDescriptor contextDesc;
		{
			contextDesc.videoMode.resolution = resolution;
			contextDesc.vsync.enabled = vsync;
			contextDesc.multiSampling.enabled = (multiSampling > 1);
			contextDesc.multiSampling.samples = multiSampling;

#if 0
			contextDesc.profileOpenGL.extProfile = true;
			contextDesc.profileOpenGL.coreProfile = true;
			contextDesc.profileOpenGL.version = LLGL::OpenGLVersion::OpenGL_3_3;
			/*contextDesc.debugCallback = [](const std::string& type, const std::string& message)
			{
			std::cout << type << " -- " << message << std::endl;
			};*/
#endif

#ifdef __linux__
			contextDesc.profileOpenGL.extProfile = true;
			contextDesc.profileOpenGL.coreProfile = true;
			contextDesc.profileOpenGL.version = LLGL::OpenGLVersion::OpenGL_3_3;
#endif
		}

		context = renderer->CreateRenderContext(contextDesc);

		// Create command buffer
		commands = renderer->CreateCommandBuffer();

		// Initialize command buffer
		commands->SetClearColor(defaultClearColor);
		commands->SetRenderTarget(*context);
		commands->SetViewport({ 0.0f, 0.0f, static_cast<float>(resolution.x), static_cast<float>(resolution.y) });
		commands->SetScissor({ 0, 0, resolution.x, resolution.y });

		// Print renderer information
		const auto& info = renderer->GetRendererInfo();

		std::cout << "renderer information:" << std::endl;
		std::cout << "  renderer:         " << info.rendererName << std::endl;
		std::cout << "  device:           " << info.deviceName << std::endl;
		std::cout << "  vendor:           " << info.vendorName << std::endl;
		std::cout << "  shading language: " << info.shadingLanguageName << std::endl;

		// Set window title
		auto& window = static_cast<LLGL::Window&>(context->GetSurface());

		auto rendererName = renderer->GetName();
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
		window.AddEventListener(std::make_shared<ResizeEventHandler>(*this, context, commands, projection));

		// Initialize default projection matrix
		projection = PerspectiveProjection(GetAspectRatio(), 0.1f, 100.0f, Gs::Deg2Rad(45.0f));

		// Show window
		window.Show();

		// Store information that loading is done
		loadingDone_ = true;
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
				auto shader = renderer->CreateShader(desc.type);

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