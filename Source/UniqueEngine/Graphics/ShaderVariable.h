#pragma once
#include "Texture.h"
#include <RefCntAutoPtr.h>

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
		};

		UniformData();
		UniformData(const UniformData& data);
		UniformData(float val);
		UniformData(const Vector2& val);
		UniformData(const Vector3& val);
		UniformData(const Vector4& val);
		UniformData(const Color& val);
		//UniformData(float* arrayPtr, int count);

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
			COLOR
			//MAT4,
			//ARRAY
		};

		Uniform();
		Uniform(const StringID& name, Uniform::Type type, const UniformData& val);
		~Uniform();

		
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
			//case Unique::Uniform::ARRAY:
			//	transfer.TransferAttribute("Value", value_.strVal_);
			//	break;
			//case Unique::Uniform::MAT4:
			//	transfer.TransferAttribute("Value", value_.strVal_);
			//	break;
			default:
				transfer.TransferAttribute("Value", value_.strVal_);
				break;
			}
			transfer.EndObject();
		}

	};

	uEnum(Uniform::Type, "INT", "FLOAT", "FLOAT2", "FLOAT3", "FLOAT4", "COLOR")

	struct TextureSlot
	{
		void SetTexture(Texture* tex);

		StringID name_;
		ResourceRef texRef_;
		SPtr<Texture> texture_;
		RefCntWeakPtr<IShaderVariable> shaderVarible_;
		
		uClass("Name", name_, "Texture", texRef_)
	};

}

