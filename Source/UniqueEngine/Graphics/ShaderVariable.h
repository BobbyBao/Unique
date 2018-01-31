#pragma once
#include "Texture.h"

namespace Unique
{
	class Texture;
	class GraphicsBuffer;

	struct UniformData
	{
		UniformData();
		UniformData(const UniformData& data);
		UniformData(float val);
		UniformData(const Vector2& val);
		UniformData(const Vector3& val);
		UniformData(const Vector4& val);
		UniformData(const Color& val);
		~UniformData();

		void operator =(const UniformData& data);

		union
		{
			int intVal_;
			float floatVal_;
			Vector2 float2Val_;
			Vector3 float3Val_;
			Vector4 float4Val_;
			Color colorVal_;
		};
		
		bool operator == ( const UniformData& rhs) const
		{
			return std::memcmp(this, &rhs, sizeof(*this)) == 0;
		}

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
		};

		Uniform();
		Uniform(const StringID& name, Uniform::Type type, const UniformData& val);
		~Uniform();

		StringID name_;
		Uniform::Type type_ = Uniform::Type::INT;
		UniformData value_;
		
		bool operator == ( const Uniform& rhs) const
		{
			return name_ == rhs.name_ && type_ == rhs.type_ && value_ == rhs.value_;
		}

		template<class VisitFunction>
		void Visit(VisitFunction& visitor)
		{
			visitor.StartObject(3);
			visitor.VisitAttribute("Name", name_);
			visitor.VisitAttribute("Type", type_);

			switch (type_)
			{
			case Unique::Uniform::INT:
				visitor.VisitAttribute("Value", value_.intVal_);
				break;
			case Unique::Uniform::FLOAT:
				visitor.VisitAttribute("Value", value_.floatVal_);
				break;
			case Unique::Uniform::FLOAT2:
				visitor.VisitAttribute("Value", value_.float2Val_);
				break;
			case Unique::Uniform::FLOAT3:
				visitor.VisitAttribute("Value", value_.float3Val_);
				break;
			case Unique::Uniform::FLOAT4:
				visitor.VisitAttribute("Value", value_.float4Val_);
				break;
			case Unique::Uniform::COLOR:
				visitor.VisitAttribute("Value", value_.colorVal_);
				break;
			default:
				visitor.VisitAttribute("Value", value_.intVal_);
				break;
			}

			visitor.EndObject();
		}

	};

	uEnum(Uniform::Type, "INT", "FLOAT", "FLOAT2", "FLOAT3", "FLOAT4", "COLOR")

	struct TextureSlot
	{
		void SetTexture(Texture* tex);
		void SetTextureAttr(const ResourceRef& texAttr);
		void Create();

		StringID name_;
		ResourceRef texAttr_;
		SPtr<Texture> texture_;
		
		bool operator == ( const TextureSlot& rhs) const
		{
			return name_ == rhs.name_ && texAttr_ == rhs.name_ && texture_ == rhs.texture_;
		}

		uClass("Name", name_, "Texture", texAttr_)
	};

}

