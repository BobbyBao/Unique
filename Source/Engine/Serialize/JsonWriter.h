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
		void TransferBasicData(T& data);

		template<class T>
		void TransferObject(SPtr<T>& data);

		template<class T>
		void TransferSTLStyleArray(T& data, int metaFlag = 0);

		template<class T>
		void TransferSTLStyleMap(T& data, int metaFlag = 0);

		template<class T>
		void TransferSTLStyleSet(T& data, int metaFlag = 0);

		bool StartObject(uint size);
		void EndObject();
	protected:
		bool StartProperty(const String& key);
		void EndProperty();
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

	inline bool JsonWriter::StartProperty(const String& key)
	{
		writer_->Key(key.CString());
		return true;
	}

	inline void JsonWriter::EndProperty()
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
	inline void JsonWriter::TransferSTLStyleArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		StartArray(data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		EndArray();
	}

	template<class T>
	inline void JsonWriter::TransferSTLStyleMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		StartArray(data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		EndArray();
	}


	template<class T>
	inline void JsonWriter::TransferSTLStyleSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		StartArray(data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		EndArray();
	}

	template<class T>
	void JsonWriter::TransferBasicData(T& data)
	{
		data.Transfer(*this);
	//	String str = ToString(data);
	//	writer_->String(str.CString());
	}

	template<>
	inline void JsonWriter::TransferBasicData<std::string>(std::string& data)
	{
		writer_->String(data.c_str());
	}

	template<>
	inline void JsonWriter::TransferBasicData<String>(String& data)
	{
		writer_->String(data.CString());
	}

	template<>
	inline void JsonWriter::TransferBasicData<bool>(bool& data)
	{
		writer_->Bool(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<char>(char& data)
	{
		writer_->Int(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<unsigned char>(unsigned char& data)
	{
		writer_->Uint(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<short>(short& data)
	{
		writer_->Int(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<unsigned short>(unsigned short& data)
	{
		writer_->Uint(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<int>(int& data)
	{
		writer_->Int(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<unsigned int>(unsigned int& data)
	{
		writer_->Uint(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<long long>(long long& data)
	{
		writer_->Int64(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<unsigned long long>(unsigned long long& data)
	{
		writer_->Uint64(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<float>(float& data)
	{
		writer_->Double(data);
	}

	template<>
	inline void JsonWriter::TransferBasicData<double>(double& data)
	{
		writer_->Double(data);
	}


}