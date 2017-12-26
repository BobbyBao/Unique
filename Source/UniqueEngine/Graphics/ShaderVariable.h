#pragma once
#include "Texture.h"

namespace Unique
{
	class Texture;
	class GraphicsBuffer;

	struct UniformData
	{
		union
		{
			int intVal_;
			float floatVal_;
			Vector2 float2Val_;
			Vector3 float3Val_;
			Vector4 float4Val_;
			Color colorVal_;
			String strVal_;
			SPtr<Texture> texture_;
		};

		UniformData();
		UniformData(const UniformData& data);
		UniformData(float val);
		UniformData(const Vector2& val);
		UniformData(const Vector3& val);
		UniformData(const Vector4& val);
		UniformData(const Color& val);
		UniformData(float* arrayPtr, int count);

		~UniformData();

		void operator =(const UniformData& data);

	};

	struct Uniform
	{
		enum Type
		{
			INT,
			FLOAT,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			COLOR,
			MAT4,
			ARRAY,
			TEXTURE
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

		RefCntWeakPtr<IShaderVariable> shaderVarible_;

		template<class TransferFunction>
		void Transfer(TransferFunction& transfer)
		{
			transfer.StartObject(3);
			transfer.TransferAttribute("Name", name_);
			transfer.TransferAttribute("Type", type_);
			switch (type_)
			{
			case Unique::Uniform::INT:
				transfer.TransferAttribute("Value", value_.intVal_);
				break;
			case Unique::Uniform::FLOAT:
				transfer.TransferAttribute("Value", value_.floatVal_);
				break;
			case Unique::Uniform::FLOAT2:
				transfer.TransferAttribute("Value", value_.float2Val_);
				break;
			case Unique::Uniform::FLOAT3:
				transfer.TransferAttribute("Value", value_.float3Val_);
				break;
			case Unique::Uniform::FLOAT4:
				transfer.TransferAttribute("Value", value_.float4Val_);
				break;
			case Unique::Uniform::COLOR:
				transfer.TransferAttribute("Value", value_.colorVal_);
				break;
			case Unique::Uniform::ARRAY:
				transfer.TransferAttribute("Value", value_.strVal_);
				break;
			case Unique::Uniform::MAT4:
				transfer.TransferAttribute("Value", value_.strVal_);
				break;
			case Unique::Uniform::TEXTURE:
				transfer.TransferAttribute("Value", value_.strVal_);
				break;
			default:
				break;
			}
			transfer.EndObject();
		}

		//uClass("Name", name_, "Type", type_, "Data", value_.vecVal_)
	};

	uEnum(Uniform::Type, "INT", "FLOAT", "VEC2", "VEC3", "VEC4")



}

