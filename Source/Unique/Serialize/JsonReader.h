#pragma once

#include "../serialize/SerializeTraits.h"
#include "../Serialize/TransferBase.h"
#include "../IO/File.h"
#include "rapidjson/rapidjson.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
#include "hjson/hjson.h"

using namespace rapidjson;

namespace Unique
{

	class JsonReader
	{
		uSerializer(JsonReader, TransferState::Reading)
	public:
		~JsonReader() {}

		template<class T>
		bool Load(const String& fileName, T& data, bool hum = false);
		
		template<class T>
		bool Load(File& source, T& data, bool hum = false);
		
		template<class T>
		void TransferPrimitive(T& data);
		
		template<class T>
		void TransferObject(SPtr<T>& data);

		template<class T>
		void TransferArray(T& data, int metaFlag = 0);

		template<class T>
		void TransferMap(T& data, int metaFlag = 0);

		template<class T>
		void TransferSet(T& data, int metaFlag = 0);
		
		bool StartObject(uint size) { return true; }

		void EndObject() {}
	private:
		bool StartAttribute(const String& key);
		void EndAttribute();
		bool StartArray(uint size) { return true; }
		void EndArray() {}

		bool human = false;
		Value* currentNode_ = nullptr;
		Vector<Value*> parentNode_;

		Hjson::Value hJsonNode_;
		Vector<Hjson::Value> hJsonParentNode_;
	};

	template<class T>
	inline	bool JsonReader::Load(const String& fileName, T& data, bool hum)
	{
		auto& cache = Subsystem<ResourceCache>();
		SPtr<File> file = cache.GetFile(fileName);
		if (!file)
		{
			return false;
		}

		return Load(*file, data, hum);
	}

	template<class T>
	inline bool JsonReader::Load(File& source, T& data, bool hum)
	{
		human = hum;

		unsigned dataSize = source.GetSize();
		if (!dataSize && !source.GetName().Empty())
		{
			UNIQUE_LOGERROR("Zero sized JSON data in " + source.GetName());
			return false;
		}

		SharedArrayPtr<char> buffer(new char[dataSize + 1]);
		if (source.Read(buffer.Get(), dataSize) != dataSize)
			return false;
		buffer[dataSize] = '\0';

		rapidjson::Document document;
		Hjson::Value hJsonRoot;
		if (human)
		{
			hJsonRoot = Hjson::Unmarshal(buffer, dataSize);
			hJsonNode_ = hJsonRoot;
		}
		else
		{
			if (document.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(buffer).HasParseError())
			{
				UNIQUE_LOGERROR("Could not parse JSON data from " + source.GetName());
				return false;
			}

			currentNode_ = &document;
		}

		SerializeTraits<T>::Transfer(data, *this);
		return true;
	}

	inline bool JsonReader::StartAttribute(const String& key)
	{
		if (human)
		{
			Hjson::Value v = hJsonNode_[key.CString()];
			if (v.empty())
			{
				return false;
			}

			hJsonParentNode_.push_back(hJsonNode_);
			hJsonNode_ = v;

		}
		else
		{
			Value::MemberIterator node = currentNode_->FindMember(key.CString());
			if (node == currentNode_->MemberEnd())
			{
				return false;
			}

			parentNode_.push_back(currentNode_);
			currentNode_ = &node->value;
		}
		return true;
	}

	inline void JsonReader::EndAttribute()
	{
		if (human)
		{
			hJsonNode_ = hJsonParentNode_.back();
			hJsonParentNode_.pop_back();
		}
		else
		{
			currentNode_ = parentNode_.back();
			parentNode_.pop_back();
		}
	}

	template<class T>
	inline void JsonReader::TransferObject(SPtr<T>& data)
	{
		if (data == nullptr)
		{
			if (human)
			{
				Hjson::Value v = hJsonNode_["Type"];
				if (v.empty())
				{
					UNIQUE_LOGWARNING("Unkown object type.");
					return;
				}

				data = StaticCast<T, Object>(GetContext()->CreateObject((const char*)v));

			}
			else
			{
				Value::MemberIterator node = currentNode_->FindMember("Type");
				if (node == currentNode_->MemberEnd())
				{
					UNIQUE_LOGWARNING("Unkown object type.");
					return;
				}

				data = StaticCast<T, Object>(GetContext()->CreateObject(node->value.GetString()));

			}
		}

		data->Transfer(*this);
	}

	template<class T>
	inline void JsonReader::TransferArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		if (human)
		{
			if (hJsonNode_.type() != Hjson::Value::VECTOR)
			{
				assert(false);
				return;
			}

			Hjson::Value parentNode = hJsonNode_;

			for (SizeType i = 0; i < parentNode.size(); ++i)
			{
				auto& child = parentNode[i];
				hJsonNode_ = child;
				non_const_value_type val;

				SerializeTraits<non_const_value_type>::Transfer(val, *this);

				data.push_back(val);
			}

			hJsonNode_ = parentNode;

		}
		else
		{
			if (!currentNode_->IsArray())
			{
				assert(false);
				return;
			}

			Value* parentNode = currentNode_;

			for (SizeType i = 0; i < parentNode->Size(); ++i)
			{
				auto& child = (*parentNode)[i];
				currentNode_ = &child;
				non_const_value_type val;

				SerializeTraits<non_const_value_type>::Transfer(val, *this);

				data.push_back(val);
			}

			currentNode_ = parentNode;
		}

	}

	template<class T>
	inline void JsonReader::TransferMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		data.clear();

		if (human)
		{
			if (hJsonNode_.type() != Hjson::Value::VECTOR)
			{
				assert(false);
				return;
			}

			Hjson::Value parentNode = hJsonNode_;
			for (SizeType i = 0; i < parentNode.size(); ++i)
			{
				auto& child = parentNode[i];
				hJsonNode_ = child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			hJsonNode_ = parentNode;
		}
		else
		{
			Value* parentNode = currentNode_;

			for (SizeType i = 0; i < parentNode->Size(); ++i)
			{
				auto& child = (*parentNode)[i];
				currentNode_ = &child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			currentNode_ = parentNode;
		}
	}

	template<class T>
	inline void JsonReader::TransferSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		if (human)
		{
			if (hJsonNode_.type() != Hjson::Value::VECTOR)
			{
				assert(false);
				return;
			}

			Hjson::Value parentNode = hJsonNode_;
			for (SizeType i = 0; i < parentNode.size(); ++i)
			{
				auto& child = parentNode[i];
				hJsonNode_ = child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			hJsonNode_ = parentNode;
		}
		else
		{
			if (!currentNode_->IsArray())
			{
				ASSERT(FALSE);
				return;
			}

			Value* parentNode = currentNode_;

			for (SizeType i = 0; i < parentNode->Size(); ++i)
			{
				auto& child = parentNode->operator[][i];
				currentNode_ = child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			currentNode_ = parentNode;
		}
	}

	template<class T>
	inline void JsonReader::TransferPrimitive(T& data)
	{
		if (human)
		{
			data = FromString<T>(String(hJsonNode_));
		}
		else
		{
			data = FromString<T>(currentNode_->GetString());
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<String>(String& data)
	{
		if (human)
		{
			data = (const char*)hJsonNode_;
		}
		else
		{
			data = currentNode_->GetString();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<bool>(bool& data)
	{
		if (human)
		{
			data = (bool)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsBool());
			data = currentNode_->GetBool();
		}

	}

	template<>
	inline void JsonReader::TransferPrimitive<char>(char& data)
	{
		if (human)
		{
			data = (char)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsInt());
			data = currentNode_->GetInt();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<char*>(char*& data)
	{
		if (human)
		{
			data = std::strcpy(data, hJsonNode_);
		}
		else
		{
			assert(currentNode_->IsString());
			std::strcpy(data, currentNode_->GetString());
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<byte>(byte& data)
	{
		if (human)
		{
			data = (byte)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsUint());
			data = currentNode_->GetUint();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<short>(short& data)
	{
		if (human)
		{
			data = (short)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsInt());
			data = currentNode_->GetInt();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<ushort>(ushort& data)
	{
		if (human)
		{
			data = (ushort)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsUint());
			data = currentNode_->GetUint();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<int>(int& data)
	{
		if (human)
		{
			data = (int)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsInt());
			data = currentNode_->GetInt();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<uint>(uint& data)
	{
		if (human)
		{
			data = (uint)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsUint());
			data = currentNode_->GetUint();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<float>(float& data)
	{
		if (human)
		{
			data = (float)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsDouble());
			data = currentNode_->GetFloat();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<double>(double& data)
	{
		if (human)
		{
			data = hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsDouble());
			data = currentNode_->GetDouble();
		}
	}



}