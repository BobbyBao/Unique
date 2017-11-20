#pragma once

#include "../serialize/SerializeTraits.h"
#include "../Serialize/TransferBase.h"
#include "../IO/File.h"
#include "rapidjson/rapidjson.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

using namespace rapidjson;

namespace Unique
{

	class JsonReader
	{
		uSerializer(JsonReader, TransferState::Reading)
	public:
		~JsonReader() {}

		template<class T>
		bool Load(const String& fileName, T& data);
		
		template<class T>
		bool Load(File& source, T& data);

		template<class T>
		void TransferBasicData(T& data);
		
		template<class T>
		void TransferObject(SPtr<T>& data);

		template<class T>
		void TransferSTLStyleArray(T& data, int metaFlag = 0);

		template<class T>
		void TransferSTLStyleMap(T& data, int metaFlag = 0);

		template<class T>
		void TransferSTLStyleSet(T& data, int metaFlag = 0);
		
		bool StartObject(uint size) { return true; }

		void EndObject() {}
	private:
		bool StartProperty(const String& key);
		void EndProperty();
		bool StartArray(uint size) { return true; }
		void EndArray() {}
		Value* currentNode = nullptr;
		Value* parentNode = nullptr;
	};

	template<class T>
	inline	bool JsonReader::Load(const String& fileName, T& data)
	{
		std::ifstream jsonFile(fileName.CString());
		IStreamWrapper isw(jsonFile);

		Document doc;
		if (doc.ParseStream(isw).HasParseError())
		{
			return false;
		}

		currentNode = &doc;
		SerializeTraits<T>::Transfer(data, *this);
		return true;
	}

	template<class T>
	inline bool JsonReader::Load(File& source, T& data)
	{
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
		if (document.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(buffer).HasParseError())
		{
			UNIQUE_LOGERROR("Could not parse JSON data from " + source.GetName());
			return false;
		}

		currentNode = &document;
		SerializeTraits<T>::Transfer(data, *this);
		return true;
	}

	inline bool JsonReader::StartProperty(const String& key)
	{
		Value::MemberIterator node = currentNode->FindMember(key.CString());
		if (node == currentNode->MemberEnd())
		{
			return false;
		}

		parentNode = currentNode;
		currentNode = &node->value;
		return true;
	}

	inline void JsonReader::EndProperty()
	{
		currentNode = parentNode;
		parentNode = nullptr;
	}

	template<class T>
	inline void JsonReader::TransferObject(SPtr<T>& data)
	{
		if (data == nullptr)
		{
			Value::MemberIterator node = currentNode->FindMember("Type");
			if (node == currentNode->MemberEnd())
			{
				UNIQUE_LOGWARNING("Unkown object type.");
				return;
			}

			data = StaticCast<T, Object>(CreateObject(node->value.GetString()));
		}

		data->Transfer(*this);
	}

	template<class T>
	inline void JsonReader::TransferSTLStyleArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		if (!currentNode->IsArray())
		{
			assert(false);
			return;
		}

		Value* parentNode = currentNode;

		for (SizeType i = 0; i < parentNode->Size(); ++i)
		{
			auto& child = (*parentNode)[i];
			currentNode = &child;
			non_const_value_type val;

			SerializeTraits<non_const_value_type>::Transfer(val, *this);

			data.push_back(val);
		}

		currentNode = parentNode;

	}

	template<class T>
	inline void JsonReader::TransferSTLStyleMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		data.clear();

		Value* parentNode = currentNode;

		for (SizeType i = 0; i < parentNode->Size(); ++i)
		{
			auto& child = (*parentNode)[i];
			currentNode = &child;
			non_const_value_type val;
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
			data.insert(val);
		}

		currentNode = parentNode;
	}

	template<class T>
	inline void JsonReader::TransferSTLStyleSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		if (!currentNode->IsArray())
		{
			ASSERT(FALSE);
			return;
		}

		Value* parentNode = currentNode;

		for (SizeType i = 0; i < parentNode->Size(); ++i)
		{
			auto& child = parentNode->operator[][i];
			currentNode = child;
			non_const_value_type val;
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
			data.insert(val);
		}

		currentNode = parentNode;
	}

	template<class T>
	inline void JsonReader::TransferBasicData(T& data)
	{
	//	data = FromString<T>(currentNode->GetString());
	}

	template<>
	inline void JsonReader::TransferBasicData<bool>(bool& data)
	{
		assert(currentNode->IsBool());
		data = currentNode->GetBool();
	}

	template<>
	inline void JsonReader::TransferBasicData<char>(char& data)
	{
		assert(currentNode->IsInt());
		data = currentNode->GetInt();
	}

	template<>
	inline void JsonReader::TransferBasicData<char*>(char*& data)
	{
		assert(currentNode->IsInt());
		strcpy(data, currentNode->GetString());
	}

	template<>
	inline void JsonReader::TransferBasicData<unsigned char>(unsigned char& data)
	{
		assert(currentNode->IsUint());
		data = currentNode->GetUint();
	}

	template<>
	inline void JsonReader::TransferBasicData<short>(short& data)
	{
		assert(currentNode->IsInt());
		data = currentNode->GetInt();
	}

	template<>
	inline void JsonReader::TransferBasicData<unsigned short>(unsigned short& data)
	{
		assert(currentNode->IsUint());
		data = currentNode->GetUint();
	}

	template<>
	inline void JsonReader::TransferBasicData<int>(int& data)
	{
		assert(currentNode->IsInt());
		data = currentNode->GetInt();
	}

	template<>
	inline void JsonReader::TransferBasicData<unsigned int>(unsigned int& data)
	{
		assert(currentNode->IsUint());
		data = currentNode->GetUint();
	}

	template<>
	inline void JsonReader::TransferBasicData<float>(float& data)
	{
		assert(currentNode->IsDouble());
		data = currentNode->GetFloat();
	}

	template<>
	inline void JsonReader::TransferBasicData<double>(double& data)
	{
		assert(currentNode->IsDouble());
		data = currentNode->GetDouble();
	}



}