#include "Precompiled.h"
#include "JsonDeserializer.h"


namespace Unique
{
	JsonDeserializer::JsonDeserializer() : Serializer(VS_READ)
	{
	}

	JsonDeserializer::~JsonDeserializer()
	{
	}

	bool JsonDeserializer::StartObject(uint size)
	{
		return true;
	}

	void JsonDeserializer::EndObject()
	{
	}

	void JsonDeserializer::VisitBin(ByteArray& data)
	{
		const char* base64 = currentNode_->GetString();
		data = FromBase64(base64, currentNode_->GetStringLength());
	}

	bool JsonDeserializer::StartDocument(const String& fileName)
	{
		document.reset(new rapidjson::Document());
		if (document->Parse<kParseCommentsFlag | kParseTrailingCommasFlag>
			(data_.data(), data_.size()).HasParseError())
		{
			UNIQUE_LOGERROR("Could not parse JSON data from " + fileName);
			return false;
		}

		currentNode_ = document.get();
		return true;
	}

	void JsonDeserializer::EndDocument()
	{
	}

	SPtr<Object> JsonDeserializer::CreateObject()
	{
		Value::MemberIterator node = currentNode_->FindMember("Type");
		if (node == currentNode_->MemberEnd())
		{
			UNIQUE_LOGWARNING("Unkown object type.");
			return nullptr;
		}

		return GetContext()->CreateObject(node->value.GetString());
	}

	bool JsonDeserializer::StartAttribute(const String& key)
	{
		Value::MemberIterator node = currentNode_->FindMember(key.CString());
		if (node == currentNode_->MemberEnd())
		{
			return false;
		}

		parentNode_.push_back(currentNode_);
		currentNode_ = &node->value;
		return true;
	}

	void JsonDeserializer::EndAttribute()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}

	bool JsonDeserializer::StartArray(uint& size)
	{
		if (!currentNode_->IsArray())
		{
			assert(false);
			return false;
		}

		size = currentNode_->Size();
		parentNode_.push_back(currentNode_);
		return true;
	}

	void JsonDeserializer::SetElement(uint index)
	{
		Value* parentNode = parentNode_.back();
		auto& child = (*parentNode)[index];
		currentNode_ = &child;
	}

	void JsonDeserializer::EndArray()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}
	
	void JsonDeserializer::VisitPrimitive(std::string& data)
	{
		data = currentNode_->GetString();
	}

	void JsonDeserializer::VisitPrimitive(String& data)
	{
		data = currentNode_->GetString();
	}

	void JsonDeserializer::VisitPrimitive(bool& data)
	{
		assert(currentNode_->IsBool());
		data = currentNode_->GetBool();
	}

	void JsonDeserializer::VisitPrimitive(char& data)
	{
		assert(currentNode_->IsInt());
		data = currentNode_->GetInt();
	}

	void JsonDeserializer::VisitPrimitive(unsigned char& data)
	{
		assert(currentNode_->IsUint());
		data = currentNode_->GetUint();
	}

	void JsonDeserializer::VisitPrimitive(short& data)
	{
		assert(currentNode_->IsInt());
		data = currentNode_->GetInt();
	}
	
	void JsonDeserializer::VisitPrimitive(unsigned short& data)
	{
		assert(currentNode_->IsUint());
		data = currentNode_->GetUint();
	}

	void JsonDeserializer::VisitPrimitive(int& data)
	{
		assert(currentNode_->IsInt());
		data = currentNode_->GetInt();
	}
	
	void JsonDeserializer::VisitPrimitive(unsigned int& data)
	{
		assert(currentNode_->IsUint());
		data = currentNode_->GetUint();
	}

	void JsonDeserializer::VisitPrimitive(long long& data)
	{
		assert(currentNode_->IsInt64());
		data = currentNode_->GetInt64();
	}

	void JsonDeserializer::VisitPrimitive(unsigned long long& data)
	{
		assert(currentNode_->IsUint64());
		data = currentNode_->GetUint64();
	}

	void JsonDeserializer::VisitPrimitive(float& data)
	{
		assert(currentNode_->IsFloat());
		data = currentNode_->GetFloat();
	}

	void JsonDeserializer::VisitPrimitive(double& data)
	{
		assert(currentNode_->IsDouble());
		data = currentNode_->GetDouble();
	}

	void JsonDeserializer::VisitPrimitive(Vector2& data)
	{
		uint size = 0;
		if (StartArray(size))
		{
			for (uint i = 0; i < size; ++i)
			{
				SetElement(i);
				switch (i)
				{
				case 0:
					TypeTraits<float>::Visit(data.x_, *this);
					break;
				case 1:
					TypeTraits<float>::Visit(data.y_, *this);
					break;
				default:
					break;
				}
				
			}

			EndArray();
		}
		
	}

	void JsonDeserializer::VisitPrimitive(Vector3& data)
	{
		uint size = 0;
		if (StartArray(size))
		{
			for (uint i = 0; i < size; ++i)
			{
				SetElement(i);
				switch (i)
				{
				case 0:
					TypeTraits<float>::Visit(data.x_, *this);
					break;
				case 1:
					TypeTraits<float>::Visit(data.y_, *this);
					break;
				case 2:
					TypeTraits<float>::Visit(data.z_, *this);
					break;
				default:
					break;
				}

			}

			EndArray();
		}
	}

	void JsonDeserializer::VisitPrimitive(Vector4& data)
	{
		uint size = 0;
		if (StartArray(size))
		{
			for (uint i = 0; i < size; ++i)
			{
				SetElement(i);
				switch (i)
				{
				case 0:
					TypeTraits<float>::Visit(data.x_, *this);
					break;
				case 1:
					TypeTraits<float>::Visit(data.y_, *this);
					break;
				case 2:
					TypeTraits<float>::Visit(data.z_, *this);
					break;
				case 3:
					TypeTraits<float>::Visit(data.w_, *this);
					break;
				default:
					break;
				}

			}

			EndArray();
		}
	}

	void JsonDeserializer::VisitPrimitive(Color& data)
	{
		uint size = 0;
		if (StartArray(size))
		{
			for (uint i = 0; i < size; ++i)
			{
				SetElement(i);
				switch (i)
				{
				case 0:
					TypeTraits<float>::Visit(data.r_, *this);
					break;
				case 1:
					TypeTraits<float>::Visit(data.g_, *this);
					break;
				case 2:
					TypeTraits<float>::Visit(data.b_, *this);
					break;
				case 3:
					TypeTraits<float>::Visit(data.a_, *this);
					break;
				default:
					break;
				}

			}

			EndArray();
		}
	}

	void JsonDeserializer::VisitPrimitive(Quaternion& data)
	{
		uint size = 0;
		if (StartArray(size))
		{
			for (uint i = 0; i < size; ++i)
			{
				SetElement(i);
				switch (i)
				{
				case 0:
					TypeTraits<float>::Visit(data.w_, *this);
					break;
				case 1:
					TypeTraits<float>::Visit(data.x_, *this);
					break;
				case 2:
					TypeTraits<float>::Visit(data.y_, *this);
					break;
				case 3:
					TypeTraits<float>::Visit(data.z_, *this);
					break;
				default:
					break;
				}

			}

			EndArray();
		}

	}
}
