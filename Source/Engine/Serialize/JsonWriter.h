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

	class JsonWriter : public TransferBase
	{
	public:
		JsonWriter() : TransferBase(TransferState::Writing)
		{
		}

		~JsonWriter()
		{
		}

		template<class T>
		bool Save(const char* fileName, T& data);
		
		template<class T>
		void Transfer(T& data, const char* name, int metaFlag = 0);

		template<class T>
		void Transfer(T& data);

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
	protected:
		int metaFlag_;
		PrettyWriter<OStreamWrapper>* writer_;
	};


	template<class T> inline
		bool JsonWriter::Save(const char* fileName, T& data)
	{
		std::ofstream jsonFile(fileName);
		OStreamWrapper osw(jsonFile);
		PrettyWriter<OStreamWrapper> writer(osw);
		writer_ = &writer;

		SerializeTraits<T>::Transfer(data, *this);

		return true;
	}

	template<class T>
	inline void JsonWriter::Transfer(T& data, const char* name, int metaFlag)
	{
		metaFlag_ = metaFlag;

		writer_->Key(name);

		SerializeTraits<T>::Transfer(data, *this);	
	}

	template<class T>
	inline void JsonWriter::Transfer(T& data)
	{
		writer_->StartObject();

		data.Transfer(*this);

		writer_->EndObject();
	}

	template<class T>
	inline void JsonWriter::TransferObject(SPtr<T>& data)
	{
		writer_->StartObject();
		data->VirtualTransfer(*this);
		writer_->EndObject();
	}

	template<class T>
	inline void JsonWriter::TransferSTLStyleArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		writer_->StartArray();

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		writer_->EndArray();
	}

	template<class T>
	inline void JsonWriter::TransferSTLStyleMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		writer_->StartArray();

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		writer_->EndArray();
	}


	template<class T>
	inline void JsonWriter::TransferSTLStyleSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		writer_->StartArray();

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		writer_->EndArray();
	}

	template<class T>
	void JsonWriter::TransferBasicData(T& data)
	{
		String str = ToString(data);
		writer_->String(str.CString());
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