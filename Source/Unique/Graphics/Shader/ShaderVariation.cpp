#include "Precompiled.h"
#include "ShaderVariation.h"
#include "../../IO/FileSystem.h"
#include "../../Resource/ResourceCache.h"
#include "Shader.h"

namespace Unique
{

	ShaderVariation::ShaderVariation(Shader& shader, const ShaderStage& type, Pass& shaderPass, uint defs)
		: owner_(shader), shaderPass_(shaderPass), shaderStage_(LLGL::ShaderType::Vertex)
	{
		shaderStage_ = type;
		mask_ = defs;

		defines_.Clear();

		for (uint i = 0; i < shaderPass_.allDefs_.size(); i++)
		{
			if (((1 << i) & defs) != 0)
			{
				if (defines_.Length() > 0)
				{
					defines_.Append(" -D");
				}

				String& def = shaderPass_.allDefs_[i];
				defines_.Append(def);
			}
		}
		
	}
	
	bool ShaderVariation::CreateImpl() 
	{
		auto& graphics = Subsystem<Graphics>();

		handle_ = renderer->CreateShader(shaderStage_.shaderType_);

		if (graphics.IsDirect3D())
		{
			// Compile shader
			LLGL::ShaderDescriptor shaderDesc(shaderStage_.entryPoint_.CString(),
				shaderStage_.target_.CString(), LLGL::ShaderCompileFlags::Debug);

			if (!handle_->Compile(shaderPass_.source_.CString(), shaderDesc))
			{
				UNIQUE_LOGERRORF(handle_->QueryInfoLog().c_str());
				return false;
			}

			return true;
		}

		String name = GetFileName(owner_.GetName());
		String extension;

		switch (shaderStage_.shaderType_)
		{
		case ShaderType::Vertex:
			extension = "_vs.glsl";
			break;
		case ShaderType::Geometry:
			extension = "_gs.glsl";
			break;
		case ShaderType::Fragment:
			extension = "_fs.glsl";
			break;
		case ShaderType::Compute:
			extension = "_cs.glsl";
			break;
		default:
			break;
		}

		String defines = defines_.Replaced(" -D", "_");

		String binaryShaderName = Shader::GetShaderPath(graphics.GetRenderID()) + name;

		if (!defines.Empty())
		{
			binaryShaderName += "_" + defines;
		}

		binaryShaderName += extension;

		if (dirty_ || !LoadByteCode(binaryShaderName))
		{
			if (!Compile(binaryShaderName))
			{
				if (LoadByteCode(Shader::GetShaderPath(graphics.GetRenderID()) + name + extension))
				{
					UNIQUE_LOGWARNING("==============================Load shader failed, name : " + binaryShaderName);
				}
				else
				{
					UNIQUE_LOGERROR("Load shader failed, name : " + binaryShaderName);
					return false;
				}
			}
			else
			{
				if (!LoadByteCode(binaryShaderName))
				{
					UNIQUE_LOGERROR("Load shader failed, name : " + binaryShaderName);
				}
			}
		}
		return true;
	}

	void ShaderVariation::Reload() 
	{ 
		dirty_ = true; 
		CreateImpl();
	}
	
	bool ShaderVariation::LoadByteCode(const String& binaryShaderName)
	{
		auto& cache = Subsystem<ResourceCache>();
		if (!cache.Exists(binaryShaderName))
			return false;

		SPtr<File> file = cache.GetFile(binaryShaderName);
		if (!file->IsOpen())
		{
			UNIQUE_LOGERROR(binaryShaderName + " is not a valid shader bytecode file");
			return false;
		}

		auto& graphics = Subsystem<Graphics>();
	
		// Compile shader
		if (graphics.IsOpenGL())
		{
			ByteArray source = file->ReadAll();
			if (!handle_->Compile(std::string(source.data(), source.size())))
			{
				UNIQUE_LOGERRORF(handle_->QueryInfoLog().c_str());
			}
		}
		else
		{
			LLGL::ShaderDescriptor shaderDesc(shaderStage_.entryPoint_.CString(), shaderStage_.target_.CString(), LLGL::ShaderCompileFlags::Debug);

			ByteArray bytes = file->ReadAll();
			if (!handle_->LoadBinary(std::move(bytes), shaderDesc))
			{
				UNIQUE_LOGERRORF(handle_->QueryInfoLog().c_str());
			}
		}

		return true;
	}

	bool ShaderVariation::Compile(const String& binaryShaderName)
	{
		auto& graphics = GetSubsystem<Graphics>();

		const String& sourceCode = shaderPass_.source_;
		if (graphics.IsDirect3D())
		{
			//to do compile hlsl to binary code
			return true;
		}

		String args;
		unsigned renderType = graphics.GetRenderID();
		bool isOpenGL = graphics.IsOpenGL();

		if (isOpenGL)
		{
		//	args.Append(" --platform linux");
		}
		else
		{
		//	args.Append(" --platform windows");
		}

		switch (shaderStage_.shaderType_)
		{
		case ShaderType::Vertex:
			args.Append(" -T vert");
			args.Append(" -E ").Append(shaderStage_.entryPoint_);
			break;
		case ShaderType::Fragment:
			args.Append(" -T frag");
			args.Append(" -E ").Append(shaderStage_.entryPoint_);
			break;
		case ShaderType::Compute:
			args.Append(" -T comp");
			args.Append(" -E ").Append(shaderStage_.entryPoint_);
			break;
		default:
			break;
		}

		FileSystem& fileSystem = Subsystem<FileSystem>();

		String inputFile = "Cache/" + Shader::GetShaderPath(RendererID::Direct3D11) 
			+ ReplaceExtension( owner_.GetName(), ".hlsl");

		{	
			fileSystem.CreateDir(GetPath(inputFile));

			File file;
			file.Open(inputFile, FILE_WRITE);
			if (!file.Write(shaderPass_.source_.CString(), shaderPass_.source_.Length()))
			{
				UNIQUE_LOGERROR("Write source file failed.");
			}
		}

		args.Append(defines_)
			.Append(" -o Cache/" + binaryShaderName)
			.Append(" -I Assets/Shaders/Common")
			.Append(" ").Append(inputFile);

		String exeFileName = "tools\\xsc.exe";

		fileSystem.CreateDir("cache/" + GetPath(binaryShaderName));

		int result = fileSystem.SystemRun(exeFileName, { args });
		if (result != 0)
		{	
			return false;
		}

		dirty_ = false;
		return true;
	}

	ShaderInstance::ShaderInstance(Shader& shader, Pass& shaderPass, unsigned defs)
	{
		if(shaderPass.vertexShader_)
		{
			SPtr<ShaderVariation> sv(new ShaderVariation(shader, shaderPass.vertexShader_, shaderPass, defs));
			shaders.push_back(sv);
		}

		if (shaderPass.pixelShader_)
		{
			SPtr<ShaderVariation> sv(new ShaderVariation(shader, shaderPass.pixelShader_, shaderPass, defs));
			shaders.push_back(sv);
		}

		if (shaderPass.computeShader_)
		{
			SPtr<ShaderVariation> sv(new ShaderVariation(shader, shaderPass.computeShader_, shaderPass, defs));
			shaders.push_back(sv);
		}

	}
	
	bool ShaderInstance::CreateImpl()
	{
		if (!handle_)
		{
			handle_ = renderer->CreateShaderProgram();
		}
		else
		{
			handle_->DetachAll();
		}

		for (auto& shader : shaders)
		{
			if (!shader->CreateImpl())
			{
				return false;
			}

			handle_->AttachShader(*shader);
		}

		dirty_ = false;
		return true;
	}

	GraphicsPipeline* ShaderInstance::GetPipeline(const VertexFormat& vertexFormat)
	{
		if (dirty_ || !IsValid())
		{
			if (!CreateImpl())
			{
				return nullptr;
			}
		} 

		if (!vertexFormat.attributes.empty())
			handle_->BuildInputLayout(vertexFormat);

		if (!handle_->LinkShaders())
			UNIQUE_LOGERRORF(handle_->QueryInfoLog().c_str());

		if (!pipeline_)
		{
			LLGL::GraphicsPipelineDescriptor pipelineDesc;
			{
				pipelineDesc.shaderProgram = handle_;
			}

			pipeline_ = renderer->CreateGraphicsPipeline(pipelineDesc);
		}

		return pipeline_;
	}

	void ShaderInstance::Reload()
	{
		dirty_ = true;

		for (auto& shd : shaders)
		{
			shd->Reload();
		}

	}

}