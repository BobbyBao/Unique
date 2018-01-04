#include "UniquePCH.h"
#include "ShaderVariable.h"

namespace Unique
{

	UniformData::UniformData() : float4Val_(0, 0, 0, 0)
	{
	}

	UniformData::UniformData(const UniformData& data)
	{
		memcpy(this, &data, sizeof(*this));
	}
	
	UniformData::UniformData(float val)
		: floatVal_(val)
	{
	}

	UniformData::UniformData(const Vector2& val)
		: float2Val_(val)
	{
	}

	UniformData::UniformData(const Vector3& val)
		: float3Val_(val)
	{
	}

	UniformData::UniformData(const Vector4& val)
		: float4Val_(val)
	{
	}

	UniformData::UniformData(const Color& val)
		: colorVal_(val)
	{
	}

	UniformData::~UniformData()
	{
	}

	void UniformData::operator =(const UniformData& data)
	{
		memcpy(this, &data, sizeof(*this));
	}

	Uniform::Uniform()
	{
	}

	Uniform::Uniform(const StringID& name, Uniform::Type type, const UniformData& val)
		: name_(name), type_(type), value_(val)
	{
	}

	Uniform::~Uniform()
	{
	}
	
	void TextureSlot::SetTexture(Texture* tex)
	{
		texture_ = tex;

		SPtr<Texture> texture = texture_;
		uCall
		(
			if (shaderVarible_.IsValid())
			{
				shaderVarible_.Lock()->Set(*texture);
			}
		);
	}

	void TextureSlot::SetTextureAttr(const ResourceRef& texAttr)
	{
		texAttr_ = texAttr;
		Texture* tex = GetSubsystem<ResourceCache>().GetResource<Texture>(texAttr.name_);
		texture_ = tex;
	}
}
