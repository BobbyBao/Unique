#include "Precompiled.h"
#include "Shader.h"
#include "Graphics/Graphics.h"

namespace Unique
{
	extern UPtr<LLGL::RenderSystem>        renderer;

	uObject(Pass)
	{
		uFactory("Graphics")
		uAttribute("Name", name_)
		uAttribute("DepthState", depthState_)
		uAttribute("ShaderDefines", allDefs_)
		uAttribute("VertexShader", vertexShader_)
		uAttribute("PixelShader", pixelShader_)
		uAttribute("ComputeShader", computeShader_)
		uAttribute("Source", source_)
	}

	uObject(Shader)
	{
		uFactory("Graphics")
		uAccessor("Name", GetName, SetName)
		uAttribute("Pass", passes_)	
	}

	Pass::Pass() : 
		vertexShader_(ShaderType::Vertex),
		pixelShader_(ShaderType::Fragment),
		computeShader_(ShaderType::Compute)
	{
	}

	Pass::~Pass()
	{
	}

	uint Pass::GetMask(Shader* shader, const String& defs)
	{
		unsigned mask = 0;
		for (uint i = 0; i < allDefs_.size(); i++)
		{
			if (defs.Find(allDefs_[i]) != String::NPOS)
			{
				mask |= (unsigned)(1 << i);
			}
		}

		return mask;
	}

	ShaderInstance* Pass::GetInstance(Shader* shader, const String& defs)
	{
		unsigned defMask = GetMask(shader, defs);

		return GetInstance(shader, defMask);
	}

	ShaderInstance* Pass::GetInstance(Shader* shader, unsigned defMask)
	{
		defMask &= allMask_;

		auto it = cachedPass_.find(defMask);;
		if (it != cachedPass_.end())
		{
			return it->second;
		}

		SPtr<ShaderInstance> inst(new ShaderInstance(*shader, *this, defMask));
		cachedPass_[defMask] = inst;
		return inst;
		
	}

	bool Pass::Prepare()
	{
		if (!computeShader_)
		{
			allDefs_ = Shader::SplitDef(computeShader_.defines_);

			if (allDefs_.size() > 0)
			{
				std::sort(allDefs_.begin(), allDefs_.end());
				allMask_ = (unsigned)(1 << (allDefs_.size() + 1)) - 1;
				computeShader_.mask_ = allMask_;
			}
		}
		else
		{
			Vector<String> psDefs = Shader::SplitDef(pixelShader_.defines_);
			Vector<String> vsDefs = Shader::SplitDef(vertexShader_.defines_);
			allDefs_ = psDefs;

			for (auto& s : vsDefs)
			{
				if (!Contains(allDefs_, s))
				{
					allDefs_.push_back(s);
				}
			}
			
			if (allDefs_.size() > 0)
			{
				std::sort(allDefs_.begin(), allDefs_.end());
				allMask_ = (unsigned)(1 << (allDefs_.size() + 1)) - 1;

				for (uint i = 0; i < allDefs_.size(); i++)
				{
					if (Find(vsDefs, allDefs_[i]) != vsDefs.end())
					{
						pixelShader_.mask_ |= (unsigned)(1 << i);
					}

					if (Find(psDefs, allDefs_[i]) != psDefs.end())
					{
						pixelShader_.mask_ |= (unsigned)(1 << i);
					}
				}
			}

		}

		return true;
	}

	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
	}

	bool Shader::Prepare()
	{
		for (auto& pass : passes_)
		{
			pass->Prepare();
		}

		return true;
	}

	bool Shader::Create()
	{
		return true;
	}

	Pass* Shader::AddPass(Pass* pass)
	{
		if (!pass)
		{
			pass = new Pass();
		}

		passes_.emplace_back(pass);
		return pass;
	}

	Pass* Shader::GetShaderPass(const StringID & passName)
	{
		for (auto& p : passes_)
		{
			if (p->name_ == passName)
			{
				return p;
			}
		}

		return nullptr;
	}

	uint Shader::GetMask(const StringID & passName, const String& defs)
	{
		Pass* pass = GetShaderPass(passName);
		if (pass == nullptr)
		{
			return 0;
		}

		return pass->GetMask(this, defs);
	}

	ShaderInstance* Shader::GetInstance(const StringID& passName, uint defMask)
	{
		Pass* pass = GetShaderPass(passName);
		if (pass == nullptr)
		{
			return nullptr;
		}

		return pass->GetInstance(this, defMask);
	}

	ShaderInstance* Shader::GetInstance(const StringID& passName, const String& defs)
	{
		Pass* pass = GetShaderPass(passName);
		if (pass == nullptr)
		{
			return nullptr;
		}

		return pass->GetInstance(this, defs);
	}

	Vector<String>/*&&*/ Shader::SplitDef(const String& defs)
	{
		if (defs.Empty())
		{
			return /*std::move*/(Vector<String>());
		}

		return defs.Split(' ');
	}

	String Shader::GetShaderPath()
	{
		return "Shader/HLSL";
	}





	Map< LLGL::ShaderProgram*, ShaderProgramRecall > shaderPrograms_;




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
			auto shaderCode = "";// ReadFileContent(desc.filename);

			// Create shader
			auto shader = renderer->CreateShader(desc.shaderType_);

			// Compile shader
			LLGL::ShaderDescriptor shaderDesc(desc.entryPoint_.CString(), desc.target_.CString(), LLGL::ShaderCompileFlags::Debug);
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

	/*
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
				auto shaderCode = "";// ReadFileContent(desc.filename);

				// Create shader
				auto shader = renderer->CreateShader(desc.type_);

				// Compile shader
				LLGL::ShaderDescriptor shaderDesc(desc.entryPoint_.CString(), desc.target_.CString(), LLGL::ShaderCompileFlags::Debug);
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
	}*/

	// Load standard shader program (with vertex- and fragment shaders)
	LLGL::ShaderProgram* LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat)
	{
		// Load shader program
		if (renderer->GetRenderingCaps().shadingLanguage >= LLGL::ShadingLanguage::HLSL_2_0)
		{
			return LoadShaderProgram(
			{
				{ LLGL::ShaderType::Vertex, "Assets/Shaders/HLSL/shader.hlsl", "VS", "vs_5_0" },
				{ LLGL::ShaderType::Fragment, "Assets/Shaders/HLSL/shader.hlsl", "PS", "ps_5_0" }
			},
				vertexFormat
			);
		}
		else
		{
			return LoadShaderProgram(
			{
				{ LLGL::ShaderType::Vertex, "Assets/Shaders/GLSL/vertex.glsl" },
				{ LLGL::ShaderType::Fragment, "Assets/Shaders/GLSL/fragment.glsl" }
			},
				vertexFormat
			);
		}
	}
}