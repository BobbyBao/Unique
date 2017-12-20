#include "UniquePCH.h"
#include "ShaderVariable.h"

namespace Unique
{

	UniformData::UniformData() : vecVal_(0, 0, 0, 0)
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
		: vec2Val_(val)
	{
	}

	UniformData::UniformData(const Vector3& val)
		: vec3Val_(val)
	{
	}

	UniformData::UniformData(const Vector4& val)
		: vecVal_(val)
	{
	}

	UniformData::UniformData(const Color& val)
		: colorVal_(val)
	{
	}

	UniformData::UniformData(float* arrayPtr, int count)
		: arrayPtr_(arrayPtr), count_(count)
	{
	}

	void UniformData::operator =(const UniformData& data)
	{
		memcpy(this, &data, sizeof(*this));
	}


	uObject(CBuffer)
	{
		uFactory("Graphics")
	}


	void ShaderVariable::Create()
	{
	}

	void ShaderVariable::Destroy()
	{
		deviceObject_ = nullptr;
	}
	
}
