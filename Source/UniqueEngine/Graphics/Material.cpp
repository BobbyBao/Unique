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
			//	uniform.shaderVarible_
				break;
			}

		}
	}

}
