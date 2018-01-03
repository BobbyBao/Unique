#include "UniquePCH.h"
#include "ShaderVariation.h"
#include "IO/FileSystem.h"
#include "Resource/ResourceCache.h"
#include "Shader.h"
#include "Graphics.h"
#include <BasicShaderSourceStreamFactory.h>

using namespace Diligent;

namespace Unique
{

	ShaderVariation::ShaderVariation(Shader& shader, const ShaderStage& type, Pass& shaderPass, uint defs)
		: owner_(shader), shaderPass_(shaderPass)
	{
		shaderStage_ = type;
		mask_ = defs;

		defines_.Clear();

		auto& graphics = GetSubsystem<Graphics>();
		
		if (graphics.IsDirect3D())
		{
			macros_.AddShaderMacro("D3D11", "");
		}

		if (type.shaderType_ == SHADER_TYPE_VERTEX || type.shaderType_ == SHADER_TYPE_PIXEL)
		{
			macros_.AddShaderMacro("COMPILEVS", "");
			macros_.AddShaderMacro("COMPILEPS", "");
		}

		for (uint i = 0; i < shaderPass_.allDefs_.size(); i++)
		{
			if (((1 << i) & defs) != 0)
			{
				if (defines_.Length() > 0)
				{
					defines_.Append("_");
				}

				const String& def = shaderPass_.allDefs_[i];
				Vector<String> m = def.Split('=', false);
				if (m.size() > 1)
				{
					defines_.Append(m[0]);
					macros_.AddShaderMacro(def.CString(), m[1].CString());
				}
				else
				{
					defines_.Append(def);
					macros_.AddShaderMacro(def.CString(), "");
				}
			}
		}

		macros_.Finalize();

		for (const auto& desc : shader.GetProperties().textureSlots_)
		{
			shaderVariableDesc_.push_back(
				ShaderVariableDesc(desc.name_.c_str(), SHADER_VARIABLE_TYPE_DYNAMIC));
		}
		
	}
	
	bool ShaderVariation::CreateImpl() 
	{
		auto& graphics = GetSubsystem<Graphics>();
		if (graphics.IsDirect3D())
		{
			if (!LoadConvertedCode("Shaders/" + shaderStage_.source_))
			{
				return false;
			}

			return true;
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

		const String& defines = defines_;
		String binaryShaderName = Shader::GetShaderPath(graphics.GetDeviceType()) + name;

		if (!defines.Empty())
		{
			binaryShaderName += "_" + defines;
		}

		binaryShaderName += extension;

		if (dirty_ || !LoadConvertedCode(binaryShaderName))
		{
			if (!Convert(binaryShaderName))
			{
				if (LoadConvertedCode(Shader::GetShaderPath(graphics.GetDeviceType()) + name + extension))
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
		auto& cache = GetSubsystem<ResourceCache>();
		if (!cache.Exists(binaryShaderName))
			return false;

		SPtr<File> file = cache.GetFile(binaryShaderName);
		if (!file->IsOpen())
		{
			UNIQUE_LOGERROR(binaryShaderName + " is not a valid shader bytecode file");
			return false;
		}
		
		source_ = file->ReadAllText();

		ShaderCreationAttribs Attrs;
		Attrs.Desc.Name = shaderStage_.source_.CString();// owner_.GetName().CString();
		Attrs.Macros = macros_;
		//Attrs.FilePath = shaderStage_.source_;
		Attrs.EntryPoint = shaderStage_.entryPoint_;
		Attrs.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
		Attrs.Source = source_.CString();
		Attrs.Desc.ShaderType = shaderStage_.shaderType_;
		Attrs.Desc.TargetProfile = SHADER_PROFILE_DX_4_0;
		Attrs.Desc.VariableDesc = shaderVariableDesc_.data();
		Attrs.Desc.NumVariables = (uint)shaderVariableDesc_.size();
		BasicShaderSourceStreamFactory BasicSSSFactory("CoreData\\Shaders;CoreData\\Shaders\\HLSL;");
		Attrs.pShaderSourceStreamFactory = &BasicSSSFactory;

		IShader* shaderObject = nullptr;
		renderDevice->CreateShader(Attrs, &shaderObject);

		shaderVariables_.clear();

		if (shaderObject)
		{
// 			const auto& desc = shaderObject->GetDesc();
// 
// 			for (uint i = 0; i < desc.NumVariables; i++)
// 			{
// 				const auto& svDesc = desc.VariableDesc[i];
// 				IShaderVariable* shaderVariable = shaderObject->GetShaderVariable(svDesc.Name);
// 				shaderVariables_[svDesc.Name] = shaderVariable;
// 			}

			deviceObject_ = shaderObject;
		}

		return deviceObject_ != nullptr;
	}

	bool ShaderVariation::Convert(const String& binaryShaderName)
	{
		auto& graphics = GetSubsystem<Graphics>();

		const String& sourceFile = shaderStage_.source_;

		String args;
		bool isOpenGL = graphics.IsOpenGL();
	
		switch (shaderStage_.shaderType_)
		{
		case ShaderType::SHADER_TYPE_VERTEX:
			args.Append(" -T vert");
			args.Append(" -E ").Append(shaderStage_.entryPoint_);
			break;
		case ShaderType::SHADER_TYPE_PIXEL:
			args.Append(" -T frag");
			args.Append(" -E ").Append(shaderStage_.entryPoint_);
			break;
		case ShaderType::SHADER_TYPE_COMPUTE:
			args.Append(" -T comp");
			args.Append(" -E ").Append(shaderStage_.entryPoint_);
			break;
		default:
			break;
		}

		FileSystem& fileSystem = GetSubsystem<FileSystem>();
	
		String inputFile = "Cache/" + Shader::GetShaderPath(DeviceType::D3D11) 
			+ ReplaceExtension( owner_.GetName(), ".hlsl");

		{	
			fileSystem.CreateDir(GetPath(inputFile));

			File file;
			file.Open(inputFile, FILE_WRITE);
			if (!file.Write(source_.CString(), source_.Length()))
			{
				UNIQUE_LOGERROR("Write source file failed.");
			}
		}

		args.Append(defines_)
			.Append(" -o Cache/" + binaryShaderName)
			.Append(" -I Assets/Shaders/Common")
			.Append(" ").Append(inputFile);

		String exeFileName = "tools\\xsc.exe";

		fileSystem.CreateDir("Cache/" + GetPath(binaryShaderName));

		int result = fileSystem.SystemRun(exeFileName, { args });
		if (result != 0)
		{	
			return false;
		}
		
		dirty_ = false;
		return true;
	}

}