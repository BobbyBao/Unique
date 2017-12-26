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

	UniformData::UniformData(float* arrayPtr, int count)
	//	: arrayPtr_(arrayPtr), count_(count)
	{
	}

	UniformData::~UniformData()
	{
	}

	void UniformData::operator =(const UniformData& data)
	{
		memcpy(this, &data, sizeof(*this));
	}

}
