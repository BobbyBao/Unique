#include "Precompiled.h"
#include "ShaderVariation.h"
#include "../../IO/FileSystem.h"
#include "../../Resource/ResourceCache.h"
#include "Shader.h"

namespace Unique
{

	ShaderVariation::ShaderVariation(Shader& shader, ShaderType type, ShaderPass& shaderPass, uint defs)
		: owner_(shader), shaderPass_(shaderPass)
	{
		type_ = type;
		mask_ = defs;

		defines_.clear();

		for (uint i = 0; i < shaderPass_.allDefs_.size(); i++)
		{
			if (((1 << i) & defs) != 0)
			{
				if (defines_.Length() > 0)
				{
					defines_.Append(';');
				}

				String& def = shaderPass_.allDefs_[i];
				defines_.Append(def);
			}
		}

		ResourceCache& cache = Subsystem<ResourceCache>();
		cache.StoreResourceDependency(&owner_, sourceFile());

	}

	String ShaderVariation::sourceFile() const
	{
		const String& shaderName = owner_.GetName();
		return ReplaceExtension(shaderName, ".hlsl");
	}

	bool ShaderVariation::CreateImpl() 
	{
		ReleaseImpl();

		String name = GetFileName(owner_.GetName());
		String extension;

		extension = (type_ == ShaderType::Vertex ? "_vs.bin" : "_fs.bin");

		String defines = defines_.Replaced(';', '_');

		String binaryShaderName = Shader::GetShaderPath() + name;

		if (!defines.Empty())
		{
			binaryShaderName += "_" + defines;
		}

		binaryShaderName += extension;
		if (dirty_ || !LoadByteCode(binaryShaderName))
		{
			if (!Compile(binaryShaderName))
			{
				if (LoadByteCode(Shader::GetShaderPath() + name + extension))
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

		if (graphics.IsOpenGL())
		{

		}
		else
		{

		}

		/*
		const bgfx::Memory* mem = bgfx::alloc(file->GetSize() + 1);
		file->Read(mem->data, mem->size);
		mem->data[file->GetSize()] = '\0';
		handle_ = bgfx::createShader(mem);

		if (!bgfx::isValid(handle_))
		{
			UNIQUE_LOGERROR("Failed to create shader.");
			return false;
		}*/

		return true;
	}

	bool ShaderVariation::Compile(const String& binaryShaderName)
	{
		String sourceCode = sourceFile();

		auto& graphics = Subsystem<Graphics>();
		if (graphics.IsOpenGL())
		{

		}
		else
		{

		}

		/*
		String args;
		unsigned renderType = bgfx::getRendererType();
		bool isOpenGL = ((renderType == bgfx::RendererType::OpenGL) 
			|| (renderType == bgfx::RendererType::OpenGLES));

		if (isOpenGL)
		{
			args.Append(" --platform linux");
		}
		else
		{
			args.Append(" --platform windows");
		}

		switch (type_)
		{
		case ShaderType::VS:
			if (isOpenGL)
			{
				args.Append(" -p 130");
			}
			else
			{
				args.Append(" -p vs_4_0");
			}
			args.Append(" --type vertex");
			break;
		case ShaderType::PS:
			if (isOpenGL)
			{
				args.Append(" -p 130");
			}
			else
			{
				args.Append(" -p ps_4_0");
			}
			args.Append(" --type fragment");
			break;
		case ShaderType::CS:
			if (isOpenGL)
			{
				args.Append(" -p 430");
			}
			else
			{
				args.Append(" -p cs_5_0");
			}
			args.Append(" --type compute");
			break;
		default:
			break;
		}

		args.Append(" --define ").Append(defines_)
			.Append(" -f CoreData/").Append(sourceCode)
			.Append(" -i CoreData/Shaders/")
			.Append(" -o cache/" + binaryShaderName);

		String exeFileName = "tools\\shadercRelease.exe";

		FileSystem& fileSystem = Subsystem<FileSystem>();

		fileSystem.CreateDir("cache/" + GetPath(binaryShaderName));

		int result = fileSystem.SystemRun(exeFileName, { args });
		if (result != 0)
		{	
			return false;
		}

		dirty_ = false;*/
		return true;
	}

	ShaderInstance::ShaderInstance(Shader& shader, ShaderPass& shaderPass, unsigned defs)
	{
		for (auto& shd : shaderPass.GetShaderStages())
		{
			SPtr<ShaderVariation> sv(new ShaderVariation(shader, shd.type, shaderPass, defs));
			shaders.push_back(sv);
		}
		
	}
	
	bool ShaderInstance::CreateImpl()
	{
		ReleaseImpl();

		for (auto& shd : shaders)
		{
			if (!shd->CreateImpl())
			{
				return false;
			}
		}

		dirty_ = false;
		return true;
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