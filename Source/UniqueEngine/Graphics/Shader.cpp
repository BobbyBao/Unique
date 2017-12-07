#include "Precompiled.h"
#include "Shader.h"
#include "Graphics/Graphics.h"

namespace Unique
{
	uObject(Pass)
	{
		uFactory("Graphics")
		uAttribute("Name", name_)
		//uAttribute("DepthState", depthState_)
		uAttribute("ShaderDefines", allDefs_)
		uAttribute("VertexShader", vertexShader_)
		uAttribute("PixelShader", pixelShader_)
		uAttribute("ComputeShader", computeShader_)
		uAttribute("Source", source_)
	}

	uObject(Shader)
	{
		uFactory("Graphics")
		uAttribute("ShaderName", shaderName_)
		uAttribute("Pass", passes_)	
	}

	Pass::Pass() : 
		vertexShader_(ShaderType::SHADER_TYPE_VERTEX),
		pixelShader_(ShaderType::SHADER_TYPE_PIXEL),
		computeShader_(ShaderType::SHADER_TYPE_COMPUTE)
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

	Vector<String> Shader::SplitDef(const String& defs)
	{
		if (defs.Empty())
		{
			return Vector<String>();
		}

		return defs.Split(' ');
	}

	String Shader::GetShaderPath(DeviceType renderID)
	{
		switch (renderID)
		{
		case DeviceType::D3D11:
		case DeviceType::D3D12:
			return "Shaders/HLSL/";
			
		case DeviceType::OpenGL:
			return "Shaders/GLSL/";
		case DeviceType::OpenGLES:
			return "Shaders/ESSL/";

		//case DeviceType::Vulkan:
		//	return "Shaders/VKSL/";
		//case DeviceType::Metal:
		//	return "Shaders/metal/";
		default:
			break;
		}

		return "";
	}



}