#include "Precompiled.h"
#include "Material.h"
#include <ResourceMapping.h>

namespace Unique
{
	uObject(Material)
	{
		uFactory("Graphics");
		uAttribute("Shader", shaderRes_);
		uAttribute("ShaderDefines", shaderDefines_);
		uAttribute("ShaderVaribles", uniforms_);
	}

	Material::Material()
	{
	}

	Material::~Material()
	{
	}
	
	void Material::CreateImpl()
	{

	}

	void Material::SetShader(const ResourceRef& shader)
	{
		shaderRes_ = shader;
		shader_ = GetSubsystem<ResourceCache>().GetResource<Shader>(shaderRes_.name_);
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
