#include "Precompiled.h"
#include "Technique.h"
#include "Graphics/Graphics.h"

namespace Unique
{
	uObject(Pass)
	{
		uFactory("Graphics")
		uAttribute("ShaderStages", shaderStages_, Vector<ShaderStage>, TF_DEFAULT)
	}

	uObject(Technique)
	{
		uFactory("Graphics")
		uAccessor("Name", GetName, SetName, String, TF_DEFAULT)
		uAttribute("ShaderDefines", shaderDefines_, String, TF_DEFAULT)
		uAttribute("ShaderPasses", passes_, Vector<SPtr<Pass>>, TF_DEFAULT)
	}

	Map< LLGL::ShaderProgram*, ShaderProgramRecall > shaderPrograms_;

	Technique::Technique()
	{
	}

	Technique::~Technique()
	{
	}

	Pass* Technique::AddPass(Pass* pass)
	{
		if (!pass)
		{
			pass = new Pass();
		}

		passes_.push_back(SPtr<Pass>(pass));
		return pass;
	}

	static std::string ReadFileContent(const String& filename)
	{
		// Read shader file
		std::ifstream file(filename.CString());

		if (!file.good())
			UNIQUE_LOGERROR("failed to open file: \"" + filename + "\"");

		return std::string(
			(std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>())
		);
	}


	LLGL::ShaderProgram* LoadShaderProgram(
		const Vector<ShaderStage>& shaderDescs,
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
			LLGL::ShaderDescriptor shaderDesc(desc.entryPoint.CString(), desc.target.CString(), LLGL::ShaderCompileFlags::Debug);
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
			UNIQUE_LOGERRORF(shaderProgram->QueryInfoLog().c_str());

		// Store information in call
		recall.vertexFormat = vertexFormat;
		recall.streamOutputFormat = streamOutputFormat;
		shaderPrograms_[shaderProgram] = recall;

		return shaderProgram;
	}

	// Reloads the specified shader program from the previously specified shader source files.
	bool ReloadShaderProgram(LLGL::ShaderProgram* shaderProgram)
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
				LLGL::ShaderDescriptor shaderDesc(desc.entryPoint.CString(), desc.target.CString(), LLGL::ShaderCompileFlags::Debug);
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
				UNIQUE_LOGERRORF(shaderProgram->QueryInfoLog().c_str());

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
	LLGL::ShaderProgram* LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat)
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