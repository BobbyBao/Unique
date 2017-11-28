#include "Precompiled.h"
#include "ShaderVariation.h"
#include "../../io/FileSystem.h"
#include "../../resource/ResourceCache.h"
#include "ShaderUtil.h"
#include "Shader.h"

namespace Unique
{

	ShaderVariation::ShaderVariation(Shader& shader, ShaderType type, ShaderPass& shaderPass, unsigned defs)
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
		switch (type_)
		{
		case ShaderType::VS:
			return ReplaceExtension(shaderName, ".vs");
		case ShaderType::PS:
			return ReplaceExtension(shaderName, ".fs");
		case ShaderType::CS:
			return ReplaceExtension(shaderName, ".cs");
		default:
			return "";
		}

	}

	bool ShaderVariation::create() 
	{
		Release();

		String name = GetFileName(owner_.GetName());
		String extension;

		extension = (type_ == ShaderType::VS ? "_vs.bin" : "_fs.bin");

		String defines = defines_.Replaced(';', '_');

		String binaryShaderName = ShaderUtil::GetShaderPath() + name;

		if (!defines.Empty())
		{
			binaryShaderName += "_" + defines;
		}

		binaryShaderName += extension;
		if (dirty_ || !loadByteCode(binaryShaderName))
		{
			if (!compile(binaryShaderName))
			{
				if (loadByteCode(ShaderUtil::GetShaderPath() + name + extension))
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
				if (!loadByteCode(binaryShaderName))
				{
					UNIQUE_LOGERROR("Load shader failed, name : " + binaryShaderName);
				}
			}
		}
		return true;
	}

	void ShaderVariation::reload() 
	{ 
		dirty_ = true; 
		create();
	}
	
	bool ShaderVariation::loadByteCode(const String& binaryShaderName)
	{
		ResourceCache& cache = Subsystem<ResourceCache>();
		if (!cache.Exists(binaryShaderName))
			return false;

		FileSystem& fileSystem = Subsystem<FileSystem>();

		SPtr<File> file = cache.GetFile(binaryShaderName);
		if (!file)
		{
			UNIQUE_LOGERROR(binaryShaderName + " is not a valid shader bytecode file");
			return false;
		}

		const bgfx::Memory* mem = bgfx::alloc(file->GetSize() + 1);
		file->Read(mem->data, mem->size);
		mem->data[file->GetSize()] = '\0';
		handle_ = bgfx::createShader(mem);

		if (!bgfx::isValid(handle_))
		{
			UNIQUE_LOGERROR("Failed to create shader.");
			return false;
		}

		return true;
	}

	bool ShaderVariation::compile(const String& binaryShaderName)
	{
		String sourceCode = sourceFile();

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

		dirty_ = false;
		return true;
	}

	ShaderInstance::ShaderInstance(Shader& shader, ShaderPass& shaderPass, unsigned defs)
	{
		cs = new ShaderVariation(shader, ShaderType::CS, shaderPass, defs);
	}

	ShaderInstance::ShaderInstance(Shader& shader, ShaderPass& shaderPass, unsigned vsDefs, unsigned psDefs)
	{
		vs = new ShaderVariation(shader, ShaderType::VS, shaderPass, vsDefs);
		ps = new ShaderVariation(shader, ShaderType::PS, shaderPass, psDefs);
	}


	bool ShaderInstance::Create()
	{
		Release();

		if (cs)
		{
			if (!cs->create())
			{
				return false;
			}

			handle_ = bgfx::createProgram(cs->handle(), false);
		}
		else
		{
			if (!vs->create())
			{
				return false;
			}

			if (!ps->create())
			{
				return false;
			}

			handle_ = bgfx::createProgram(vs->handle(), ps->handle(), false);
		}

		dirty_ = false;
		return true;
	}

	void ShaderInstance::reload()
	{
		dirty_ = true;

		if (cs)
		{
			cs->reload();
		}
		else
		{
			vs->reload();
			ps->reload();
		}
	}

}