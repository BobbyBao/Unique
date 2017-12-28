#include "Precompiled.h"
#include "Material.h"
#include <ResourceMapping.h>

namespace Unique
{
	uObject(Material)
	{
		uFactory("Graphics");
		uAttribute("Shader", shaderAttr_);
		uAttribute("ShaderDefines", shaderDefines_);
		uAttribute("Uniforms", uniforms_); 
		uAttribute("TextureSlots", textureSlots_);
	}

	Material::Material()
	{
	}

	Material::~Material()
	{
	}
	
	bool Material::Prepare()
	{
		auto& cache = GetSubsystem<ResourceCache>();

		if (GetAsyncLoadState() == ASYNC_LOADING)
		{
		//	cache.BackgroundLoadResource<Shader>(shaderAttr_, true, this);
		}

		if (shader_)
		{
			uniforms_ = shader_->GetUniforms();
			textureSlots_ = shader_->GetTextureSlots();
		}

		return true;
	}

	void Material::SetShader(const ResourceRef& shader)
	{
		shaderAttr_ = shader;
		shader_ = GetSubsystem<ResourceCache>().GetResource<Shader>(shaderAttr_.name_);
	}

	void Material::SetTexture(const StringID& name, Texture* texture)
	{
		for (auto& ts : textureSlots_)
		{
			if (ts.name_ == name)
			{
				ts.texture_ = texture;
				break;
			}

		}
	}

	void Material::Apply()
	{
		for (auto& uniform : uniforms_)
		{
			if (uniform.shaderVarible_.IsValid())
			{
			//	uniform.shaderVarible_.Lock()->Set();
			}

		}

		for (auto& ts : textureSlots_)
		{
			if (ts.shaderVarible_.IsValid())
			{
				ts.shaderVarible_.Lock()->Set(*ts.texture_);
			}

		}
	}

}
