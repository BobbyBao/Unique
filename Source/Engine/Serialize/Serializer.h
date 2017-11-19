#pragma once

#include "../serialize/SerializeTraits.h"
#include "../Serialize/mpack/mpack.h"
#include <fstream>

namespace Unique
{
	class Serializer
	{
	public:

		bool IsReading() { return false; }
		bool IsWriting() { return true; }

		template<class T>
		bool Save(const char* fileName, T& data);

		template<class T>
		void Transfer(T& data, const char* name, int metaFlag = 0);

		int size_ = 0;
		bool inMap_ = false;
		void BeginMap(int sz)
		{
			inMap_ = true;
			size_ = sz;
			mpack_start_map(&writer_, size_);
		}

		void EndMap()
		{
			mpack_finish_map(&writer_);
			inMap_ = false;
		}
		
		template <typename First, typename... Rest>
		void TransferNVP(First& first, Rest&... rest)
		{
			int sz = sizeof ...(Rest)+1;
			BeginMap(sz / 2);
			TransferImpl1(first, rest...); // recursive call using pack expansion syntax  
			EndMap();
		}

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

		template <typename First, typename... Rest>
		void TransferImpl1(First& first, Rest&... rest)
		{
			TransferImpl2(first, rest...); // recursive call using pack expansion syntax  
		}

		template <typename First, typename Second, typename... Rest>
		void TransferImpl2(First& val, Second name, const Rest&... rest)
		{
			Transfer(val, name);
			TransferImpl1(rest...); // recursive call using pack expansion syntax  
		}

		template <typename First, typename Second, typename... Rest>
		void TransferImpl2(First& val, Second name) {
			Transfer(val, name);
		}

		int metaFlag_;
		mpack_writer_t writer_;
	};

	template<class T>
	inline bool Serializer::Save(const char* fileName, T& data)
	{
		char* buff;
		size_t size;
		mpack_writer_init_growable(&writer_, &buff, &size);

		SerializeTraits<T>::Transfer(data, *this);
		// finish writing
		if (mpack_writer_destroy(&writer_) != mpack_ok)
		{
		//	UNIQUE_LOGERROR("An error occurred encoding the data!");
			return false;
		}

		std::ofstream packFile(fileName);
		packFile.write(buff, size);
		return true;
	}

	template<class T>
	inline void Serializer::Transfer(T& data, const char* name, int metaFlag)
	{
		metaFlag_ = metaFlag;

		//	writer_->Key(name);

		mpack_write_str(&writer_, name, (uint)std::strlen(name));

		SerializeTraits<T>::Transfer(data, *this);
	}

	template<class T>
	inline void Serializer::Transfer(T& data)
	{
		//	writer_->StartObject();
		//mpack_start_map(&writer_, 0);
		data.Transfer(*this);
		//mpack_finish_map(&writer_);
		//	writer_->EndObject();
	}

	template<class T>
	inline void Serializer::TransferObject(SPtr<T>& data)
	{
		//	writer_->StartObject();

		//mpack_start_map(&writer_, 3);

		data->Transfer(*this);

		//mpack_finish_map(&writer_);

		//	writer_->EndObject();
	}

	template<class T>
	inline void Serializer::TransferSTLStyleArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		//	writer_->StartArray();

		mpack_start_array(&writer_, 2);
		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		mpack_finish_array(&writer_);
		//	writer_->EndArray();
	}

	template<class T>
	inline void Serializer::TransferSTLStyleMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		//	writer_->StartArray();

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		//	writer_->EndArray();
	}


	template<class T>
	inline void Serializer::TransferSTLStyleSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		//	writer_->StartArray();

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		//	writer_->EndArray();
	}

	template<class T>
	void Serializer::TransferBasicData(T& data)
	{
		String str = ToString(data);
		//	writer_->String(str.CString());
	}

	template<>
	inline void Serializer::TransferBasicData<String>(String& data)
	{
		mpack_write_str(&writer_, data.CString(), data.Length());
		//	writer_->String(data.CString());
	}

	template<>
	inline void Serializer::TransferBasicData<bool>(bool& data)
	{
		//	writer_->Bool(data);
	}

	template<>
	inline void Serializer::TransferBasicData<char>(char& data)
	{
		//	writer_->Int(data);
	}

	template<>
	inline void Serializer::TransferBasicData<unsigned char>(unsigned char& data)
	{
		//	writer_->Uint(data);
	}

	template<>
	inline void Serializer::TransferBasicData<short>(short& data)
	{
		//	writer_->Int(data);
	}

	template<>
	inline void Serializer::TransferBasicData<unsigned short>(unsigned short& data)
	{
		//	writer_->Uint(data);
	}


	template<>
	inline void Serializer::TransferBasicData<int>(int& data)
	{
		//	writer_->Int(data);
	}

	template<>
	inline void Serializer::TransferBasicData<unsigned int>(unsigned int& data)
	{
		//	writer_->Uint(data);
	}

	template<>
	inline void Serializer::TransferBasicData<long long>(long long& data)
	{
		//	writer_->Int64(data);
	}

	template<>
	inline void Serializer::TransferBasicData<unsigned long long>(unsigned long long& data)
	{
		//	writer_->Uint64(data);
	}

	template<>
	inline void Serializer::TransferBasicData<float>(float& data)
	{
		//	writer_->Double(data);
	}

	template<>
	inline void Serializer::TransferBasicData<double>(double& data)
	{
		//	writer_->Double(data);
	}

}


#include "Serializer.inl"