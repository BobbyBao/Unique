#include "UniquePCH.h"
#include "ShaderVariation.h"
#include "IO/FileSystem.h"
#include "Resource/ResourceCache.h"
#include "Shader.h"
#include "Graphics.h"

namespace Unique
{

	ShaderVariation::ShaderVariation(Shader& shader, const ShaderStage& type, Pass& shaderPass, uint defs)
		: owner_(shader), shaderPass_(shaderPass)
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
					defines_.Append(";");
				}

				String& def = shaderPass_.allDefs_[i];
				defines_.Append(def);
			}
		}

	//	ShaderMacroHelper
		
	}
	
	bool ShaderVariation::CreateImpl() 
	{
		ShaderCreationAttribs Attrs;
		//Attrs.Desc.Name = "MainVS";
		Attrs.FilePath = shaderStage_.source_;
		Attrs.EntryPoint = shaderStage_.entryPoint_;
		Attrs.Desc.ShaderType = shaderStage_.shaderType_;
		Attrs.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
		Attrs.Desc.TargetProfile = SHADER_PROFILE_DX_4_0;
		renderDevice->CreateShader(Attrs, &handle_);

		return handle_ != nullptr;

		// to do :
		auto& graphics = Subsystem<Graphics>();
		if (graphics.IsDirect3D())
		{
			return handle_ != nullptr;
		}

		String name = GetFileName(owner_.GetName());
		String extension;

		switch (shaderStage_.shaderType_)
		{
		case ShaderType::SHADER_TYPE_VERTEX:
			extension = "_vs.glsl";
			break;
		case ShaderType::SHADER_TYPE_GEOMETRY:
			extension = "_gs.glsl";
			break;
		case ShaderType::SHADER_TYPE_PIXEL:
			extension = "_fs.glsl";
			break;
		case ShaderType::SHADER_TYPE_COMPUTE:
			extension = "_cs.glsl";
			break;
		default:
			break;
		}

		String& defines = defines_;
		String binaryShaderName = "";// Shader::GetShaderPath(graphics.GetDeviceType()) + name;

		if (!defines.Empty())
		{
			binaryShaderName += "_" + defines;
		}

		binaryShaderName += extension;

		if (dirty_ || !LoadConvertedCode(binaryShaderName))
		{
			if (!Convert(binaryShaderName))
			{
				if (LoadConvertedCode(/*Shader::GetShaderPath(graphics.GetDeviceType()) +*/ name + extension))
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
				if (!LoadConvertedCode(binaryShaderName))
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
	
	bool ShaderVariation::LoadConvertedCode(const String& binaryShaderName)
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
		//to do:
		return true;
	}

	bool ShaderVariation::Convert(const String& binaryShaderName)
	{
		auto& graphics = GetSubsystem<Graphics>();

		const String& sourceFile = shaderStage_.source_;
		/*
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
		*/
		dirty_ = false;
		return true;
	}

	PipelineState::PipelineState(Shader& shader, Pass& shaderPass, unsigned defs) : shaderPass_(shaderPass)
	{
		if (shaderPass.shaderStage_[5])
		{
			isComputePipeline_ = true;
		}
		else
		{
			isComputePipeline_ = false;

			for (int i = 0; i < 5; i++)
			{
				if (shaderPass.shaderStage_[i])
				{
					SPtr<ShaderVariation> sv(new ShaderVariation(shader, shaderPass.shaderStage_[i], shaderPass, defs));
					shaders.push_back(sv);
				}
			}

		}

	}
	
	bool PipelineState::CreateImpl()
	{
		for (auto& shader : shaders)
		{
			if (!shader->CreateImpl())
			{
				return false;
			}

		}

		PipelineStateDesc PSODesc;
		PSODesc.IsComputePipeline = isComputePipeline_;
		PSODesc.GraphicsPipeline.NumRenderTargets = 1;
		PSODesc.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_RGBA8_UNORM_SRGB;
		PSODesc.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT;

		PSODesc.GraphicsPipeline.DepthStencilDesc = shaderPass_.depthState_;
		PSODesc.GraphicsPipeline.RasterizerDesc = shaderPass_.rasterizerState_;
		PSODesc.GraphicsPipeline.BlendDesc = shaderPass_.blendState_;

		PSODesc.GraphicsPipeline.InputLayout.LayoutElements = shaderPass_.inputLayout_.data();
		PSODesc.GraphicsPipeline.InputLayout.NumElements = (uint)shaderPass_.inputLayout_.size();

		PSODesc.GraphicsPipeline.PrimitiveTopologyType = PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		
		if (isComputePipeline_)
		{
			for (auto shader : shaders)
			{
				if (shader->GetShaderType() == SHADER_TYPE_COMPUTE)
				{
					PSODesc.ComputePipeline.pCS = shader->GetHandle();
					break;
				}
			}
		}
		else
		{
			for (auto shader : shaders)
			{
				switch (shader->GetShaderType())
				{
				case SHADER_TYPE_VERTEX:
					PSODesc.GraphicsPipeline.pVS = shader->GetHandle();
					break;
				case SHADER_TYPE_PIXEL:
					PSODesc.GraphicsPipeline.pPS = shader->GetHandle();
					break;
				case SHADER_TYPE_GEOMETRY:
					PSODesc.GraphicsPipeline.pGS = shader->GetHandle();
					break;
				case SHADER_TYPE_HULL:
					PSODesc.GraphicsPipeline.pHS = shader->GetHandle();
					break;
				case SHADER_TYPE_DOMAIN:
					PSODesc.GraphicsPipeline.pDS = shader->GetHandle();
					break;
				default:
					break;
				}

			}
		}

		renderDevice->CreatePipelineState(PSODesc, &handle_);
		handle_->CreateShaderResourceBinding(&shaderResourceBinding_);
		dirty_ = false;
		return true;
	}

	IPipelineState* PipelineState::GetPipeline()
	{
		if (dirty_ || !IsValid())
		{
			if (!CreateImpl())
			{
				return nullptr;
			}
		} 

		return handle_;
	}

	void PipelineState::Reload()
	{
		dirty_ = true;

		for (auto& shd : shaders)
		{
			shd->Reload();
		}

	}

}