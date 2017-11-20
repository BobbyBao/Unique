#pragma once

#include "../serialize/SerializeTraits.h"
#include "../Serialize/TransferBase.h"
#include "../Serialize/mpack/mpack.h"
#include <fstream>

namespace Unique
{
	class BinaryWriter
	{
		uSerializer(BinaryWriter, TransferState::Writing)
	public:
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
		
		bool StartObject(uint sz)
		{
			inMap_ = true;
			size_ = sz;
			mpack_start_map(&writer_, size_);
			return true;
		}

		void EndObject()
		{
			mpack_finish_map(&writer_);
			inMap_ = false;
		}
	protected:


		bool StartProperty(const String& key)
		{
			mpack_write_str(&writer_, key.CString(), (uint)key.Length());
			return true; 
		}

		void EndProperty()
		{
		}

		bool StartArray(uint sz)
		{
			mpack_start_array(&writer_, sz);
			return true;
		}

		void EndArray()
		{
			mpack_finish_array(&writer_);
		}

		int size_ = 0;
		bool inMap_ = false;
		mpack_writer_t writer_;
	};

	template<class T>
	inline bool BinaryWriter::Save(const char* fileName, T& data)
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
	inline void BinaryWriter::TransferObject(SPtr<T>& data)
	{
		data->Transfer(*this);
	}

	template<class T>
	inline void BinaryWriter::TransferSTLStyleArray(T& data, int metaFlag)
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
	inline void BinaryWriter::TransferSTLStyleMap(T& data, int metaFlag)
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
	inline void BinaryWriter::TransferSTLStyleSet(T& data, int metaFlag)
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
	void BinaryWriter::TransferBasicData(T& data)
	{
		mpack_write_bin(&writer_, (const char*)&data, (uint)sizeof(T));
	}

	template<>
	inline void BinaryWriter::TransferBasicData<String>(String& data)
	{
		mpack_write_str(&writer_, data.CString(), data.Length());
	}

	template<>
	inline void BinaryWriter::TransferBasicData<bool>(bool& data)
	{
		mpack_write_bool(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<char>(char& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<unsigned char>(unsigned char& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<short>(short& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<unsigned short>(unsigned short& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<int>(int& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<unsigned int>(unsigned int& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<long long>(long long& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<unsigned long long>(unsigned long long& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<float>(float& data)
	{
		mpack_write(&writer_, data);
	}

	template<>
	inline void BinaryWriter::TransferBasicData<double>(double& data)
	{
		mpack_write(&writer_, data);
	}

}
