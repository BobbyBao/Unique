#include "Precompiled.h"
#include "Material.h"
#include <ResourceMapping.h>
#include "PipelineState.h"

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
			cache.BackgroundLoadResource<Shader>(shaderAttr_.name_, true, this);
		}
		else
		{
			shader_ = cache.GetResource<Shader>(shaderAttr_.name_);
		}

		return true;
	}

	bool Material::Create()
	{
		if (shader_)
		{
			//uniforms_ = shader_->GetUniforms();
			//textureSlots_ = shader_->GetTextureSlots();
			
			for (auto& sl : textureSlots_)
			{
				sl.Create();
			}
		}

		return true;
	}

	void Material::SetShaderAttr(const ResourceRef& shader)
	{
		shaderAttr_ = shader;
		shader_ = GetSubsystem<ResourceCache>().GetResource<Shader>(shaderAttr_.name_);
	}


	void Material::SetTexture(const StringID& name, const ResourceRef& textureRef)
	{
		for (auto& ts : textureSlots_)
		{
			if (ts.name_ == name)
			{
				ts.SetTextureAttr(textureRef);
				return;
			}

		}

		TextureSlot textureSlot;
		textureSlot.name_ = name;
		textureSlot.SetTextureAttr(textureRef);
		textureSlots_.push_back(textureSlot);

	}

	void Material::SetTexture(const StringID& name, Texture* texture)
	{
		for (auto& ts : textureSlots_)
		{
			if (ts.name_ == name)
			{
				ts.texture_ = texture;
				return;
			}

		}

		TextureSlot textureSlot;
		textureSlot.name_ = name;
		textureSlot.texture_ = texture;
		textureSlots_.push_back(textureSlot);
	}

	Pass* Material::GetPass(const String & pass)
	{
		auto shader = GetShader();
		return shader ? shader->GetPass(pass) : nullptr;
	}

	Pass* Material::GetPass(unsigned passIndex)
	{
		auto shader = GetShader();
		return shader ? shader->GetPass(passIndex) : nullptr;
	}

	PipelineState* Material::GetPipeline(const String& passName, const String & defs)
	{
		auto shader = GetShader();
		return shader ? shader->GetPipeline(passName, defs) : nullptr;
	}

	void Material::Apply(PipelineState* pipeline)
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
			auto sv = pipeline->GetShaderVariable(ts.name_);
			if (sv)
			{
				sv->Set(*ts.texture_->GetShaderResourceView());
			}
		}
	}

}
