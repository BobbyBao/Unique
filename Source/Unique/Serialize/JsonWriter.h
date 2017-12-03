#pragma once
#include <fstream>
#include <iosfwd>
#include "../serialize/SerializeTraits.h"
#include "../Serialize/TransferBase.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>

using namespace rapidjson;

namespace Unique
{
	class JsonWriter
	{
		uSerializer(JsonWriter, TransferState::Writing)
	public:
		~JsonWriter()
		{
		}

		template<class T>
		bool Save(const char* fileName, T& data);
		
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

		void TransferBin(ByteArray& data) {}

		bool StartObject(uint size);
		void EndObject();
	protected:
		bool StartAttribute(const String& key);
		void EndAttribute();
		bool StartArray(uint size);
		void EndArray();

		PrettyWriter<OStreamWrapper>* writer_;
	};

	template<class T> 
	inline bool JsonWriter::Save(const char* fileName, T& data)
	{
		std::ofstream jsonFile(fileName);
		OStreamWrapper osw(jsonFile);
		PrettyWriter<OStreamWrapper> writer(osw);
		writer_ = &writer;

		SerializeTraits<T>::Transfer(data, *this);

		return true;
	}

	inline bool JsonWriter::StartObject(uint size)
	{
		writer_->StartObject();
		return true; 
	}

	inline void JsonWriter::EndObject()
	{
		writer_->EndObject();
	}

	inline bool JsonWriter::StartAttribute(const String& key)
	{
		writer_->Key(key.CString());
		return true;
	}

	inline void JsonWriter::EndAttribute()
	{
	}

	inline bool JsonWriter::StartArray(uint size)
	{
		writer_->StartArray();
		return true;
	}

	inline void JsonWriter::EndArray() 
	{
		writer_->EndArray();
	}

	template<class T>
	inline void JsonWriter::TransferObject(SPtr<T>& data)
	{
		data->Transfer(*this);
	}

	template<class T>
	inline void JsonWriter::TransferArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		StartArray((uint)data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		EndArray();
	}

	template<class T>
	inline void JsonWriter::TransferMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		StartArray((uint)data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		EndArray();
	}


	template<class T>
	inline void JsonWriter::TransferSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		StartArray((uint)data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		EndArray();
	}

	template<class T>
	void JsonWriter::TransferPrimitive(T& data)
	{
		data.Transfer(*this);
	//	String str = ToString(data);
	//	writer_->String(str.CString());
	}

	template<>
	inline void JsonWriter::TransferPrimitive<std::string>(std::string& data)
	{
		writer_->String(data.c_str());
	}

	template<>
	inline void JsonWriter::TransferPrimitive<String>(String& data)
	{
		writer_->String(data.CString());
	}

	template<>
	inline void JsonWriter::TransferPrimitive<bool>(bool& data)
	{
		writer_->Bool(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<char>(char& data)
	{
		writer_->Int(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<unsigned char>(unsigned char& data)
	{
		writer_->Uint(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<short>(short& data)
	{
		writer_->Int(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<unsigned short>(unsigned short& data)
	{
		writer_->Uint(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<int>(int& data)
	{
		writer_->Int(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<unsigned int>(unsigned int& data)
	{
		writer_->Uint(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<long long>(long long& data)
	{
		writer_->Int64(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<unsigned long long>(unsigned long long& data)
	{
		writer_->Uint64(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<float>(float& data)
	{
		writer_->Double(data);
	}

	template<>
	inline void JsonWriter::TransferPrimitive<double>(double& data)
	{
		writer_->Double(data);
	}


}