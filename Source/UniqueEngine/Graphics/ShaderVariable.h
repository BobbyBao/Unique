#pragma once

namespace Unique
{
	class Texture;

	struct UniformData
	{
		union
		{
			float floatVal_;
			Vector2 vec2Val_;
			Vector3 vec3Val_;
			Vector4 vecVal_;
			Color colorVal_;
			struct
			{
				float* arrayPtr_;
				int count_;
			};
		};

		UniformData();
		UniformData(const UniformData& data);
		UniformData(float val);
		UniformData(const Vector2& val);
		UniformData(const Vector3& val);
		UniformData(const Vector4& val);
		UniformData(const Color& val);
		UniformData(float* arrayPtr, int count);

		~UniformData()
		{
		}

		void operator =(const UniformData& data);
	};

	struct Uniform
	{
		enum Type
		{
			INT,
			FLOAT,
			VEC2,
			VEC3,
			VEC4,
			COLOR,
			MAT4,
			ARRAY,
		};

		Uniform()
		{
		}

		Uniform(const StringID& name, Uniform::Type type, const UniformData& val)
			: name_(name), type_(type), value_(val)
		{
		}

		~Uniform()
		{
		}
		
		StringID name_;
		Uniform::Type type_ = Uniform::Type::INT;
		UniformData value_;
		int offset = -1;
	};

	class CBuffer : public Object
	{
		uRTTI(CBuffer, Object)
	public:
		Vector<Uniform> uniforms_;
		Vector<byte> buffer_[2];
	};


	enum class ShaderVariableType
	{
		CBuffer,
		Texture,
		Sampler,
		Buffer
	};

	uEnumTraits(ShaderVariableType, "Uniform", "Texture", "Sampler", "Buffer")

	struct ShaderVariable
	{
		ShaderVariable()
		{
		}

		ShaderVariable(const StringID& name, ShaderVariableType type, CBuffer* val)
			: name_(name), type_(type), uniformBlock_(val)
		{
		}

		~ShaderVariable()
		{
		}
		
		void Create();
		void Destroy();

		StringID name_;
		ShaderVariableType type_;
		UPtr<CBuffer> uniformBlock_;
		RefCntWeakPtr<IShaderVariable> deviceObject_;

		uClass("Name", name_, "Type", type_, "CBuffer", uniformBlock_)
	};


}

