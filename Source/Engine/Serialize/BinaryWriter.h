#pragma once

#include "../serialize/SerializeTraits.h"
#include "../Serialize/mpack/mpack.h"
#include <fstream>

namespace Unique
{
	class BinaryWriter : public TransferBase
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
	inline void BinaryWriter::Transfer(T& data, const char* name, int metaFlag)
	{
		metaFlag_ = metaFlag;

		//	writer_->Key(name);

		mpack_write_str(&writer_, name, (uint)std::strlen(name));

		SerializeTraits<T>::Transfer(data, *this);
	}

	template<class T>
	inline void BinaryWriter::Transfer(T& data)
	{
		data.Transfer(*this);
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

		mpack_start_array(&writer_, (uint)data.size());
		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		mpack_finish_array(&writer_);

	}

	template<class T>
	inline void BinaryWriter::TransferSTLStyleMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		mpack_start_array(&writer_, (uint)data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		mpack_finish_array(&writer_);
	}


	template<class T>
	inline void BinaryWriter::TransferSTLStyleSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		mpack_start_array(&writer_, (uint)data.size());

		for (non_const_value_type& val : data)
		{
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
		}

		mpack_finish_array(&writer_);
	}

	template<class T>
	void BinaryWriter::TransferBasicData(T& data)
	{
		mpack_write_bin(&writer_, &data, (uint)sizeof(T));
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
