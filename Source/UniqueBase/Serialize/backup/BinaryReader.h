#pragma once
#include "../serialize/SerializeTraits.h"
#include "../Serialize/TransferBase.h"
#include "../Serialize/mpack/mpack.h"

namespace Unique
{
	class BinaryReader
	{
		uSerializer(BinaryReader, TransferState::Reading)
	public:
		template<class T>
		bool Load(const String& fileName, T& data);

		template<class T>
		bool Load(File& source, T& data);
		
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

		void TransferBin(ByteArray& data);

		bool StartObject(uint size) { return true; }
		void EndObject() {}
	protected:
		bool StartAttribute(const String& key);
		void EndAttribute();
		bool StartArray(uint size) { return true; }
		void EndArray() {}

		mpack_tree_t tree_;
		mpack_node_t currentNode_;
		Vector<mpack_node_t> parentNode_;
	};
	
	template<class T>
	inline	bool BinaryReader::Load(const String& fileName, T& data)
	{
		File file(fileName);
		return Load(file, data);
	}

	template<class T>
	inline bool BinaryReader::Load(File& source, T& data)
	{
		uint dataSize = source.GetSize();
		if (!dataSize && !source.GetName().Empty())
		{
			UNIQUE_LOGERROR("Zero sized JSON data in " + source.GetName());
			return false;
		}

		SharedArrayPtr<char> buffer(new char[dataSize]);
		if (source.Read(buffer.Get(), dataSize) != dataSize)
			return false;

		mpack_tree_init(&tree_, buffer, dataSize);
		mpack_tree_parse(&tree_);

		if (mpack_tree_error(&tree_) != mpack_ok)
		{
			UNIQUE_LOGERROR("Could not parse MsgPack data from " + source.GetName());
			return false;
		}

		currentNode_ = mpack_tree_root(&tree_);

		SerializeTraits<T>::Transfer(data, *this);
		return true;
	}



	inline bool BinaryReader::StartAttribute(const String& key)
	{
		mpack_node_t node = mpack_node_map_str(currentNode_, key.CString(), key.Length());
		if (mpack_node_type(node) == mpack_type_nil)
		{
			return false;
		}

		parentNode_.push_back(currentNode_);
		currentNode_ = node;
		return true; 
	}

	inline void BinaryReader::EndAttribute()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}

	template<class T>
	inline void BinaryReader::TransferObject(SPtr<T>& data)
	{
		if(data == nullptr)
		{
			mpack_node_t node = mpack_node_map_cstr(currentNode_, "Type");
			if (mpack_node_type(node) == mpack_type_nil)
			{
				return;
			}

			const char* type = mpack_node_str(node);
			if (!type)
			{
				return;
			}

			size_t sz = mpack_node_strlen(node);
			data = StaticCast<T, Object>(GetContext()->CreateObject(String(type, (uint)sz)));
		}

		data->Transfer(*this);
	}

	template<class T>
	inline void BinaryReader::TransferArray(T& data, int metaFlag)
	{
		if (mpack_node_type(currentNode_) != mpack_type_array)
		{
			return;
		}

		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();
		
		mpack_node_t parentNode = currentNode_;

		for (size_t i = 0; i < mpack_node_array_length(parentNode); ++i)
		{
			currentNode_ = mpack_node_array_at(parentNode, i);
			non_const_value_type val;

			SerializeTraits<non_const_value_type>::Transfer(val, *this);

			data.push_back(val);
		}

		currentNode_ = parentNode;

	}

	template<class T>
	inline void BinaryReader::TransferMap(T& data, int metaFlag)
	{
		if (mpack_node_type(currentNode_) != mpack_type_array)
		{
			return;
		}

		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		data.clear();

		mpack_node_t parentNode = currentNode_;

		for (size_t i = 0; i < mpack_node_array_length(parentNode); ++i)
		{
			currentNode_ = mpack_node_array_at(parentNode, i);
			non_const_value_type val;
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
			data.insert(val);
		}

		currentNode_ = parentNode;
	}

	template<class T>
	inline void BinaryReader::TransferSet(T& data, int metaFlag)
	{
		if (mpack_node_type(currentNode_) != mpack_type_array)
		{
			return;
		}

		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		mpack_node_t parentNode = currentNode_;
		
		for (size_t i = 0; i < mpack_node_array_length(parentNode); ++i)
		{
			currentNode_ = mpack_node_array_at(parentNode, i);
			non_const_value_type val;
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
			data.insert(val);
		}

		currentNode_ = parentNode;
	}

	inline void BinaryReader::TransferBin(ByteArray& data)
	{
		uint size = mpack_node_data_len(currentNode_);

		data.resize(size);

		if (size != mpack_node_copy_data(currentNode_, data.data(), size))
		{
			assert(false);
		}
	}

	template<class T>
	inline void BinaryReader::TransferPrimitive(T& data)
	{
		data.Transfer(*this);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<bool>(bool& data)
	{
		data = mpack_node_bool(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<char>(char& data)
	{
		data = mpack_node_i8(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<String>(String& data)
	{
		const char* str = mpack_node_str(currentNode_);
		if (str)
		{
			size_t sz = mpack_node_strlen(currentNode_);
			data = String(str, (uint)sz);
		}
		else
		{
			data.Clear();
		}
	}

	template<>
	inline void BinaryReader::TransferPrimitive<std::string>(std::string& data)
	{
		const char* str = mpack_node_str(currentNode_);
		if (str)
		{
			size_t sz = mpack_node_strlen(currentNode_);
			data = std::string(str, sz);
		}
		else
		{
			data.clear();
		}
	}

	template<>
	inline void BinaryReader::TransferPrimitive<char*>(char*& data)
	{
		std::strcpy(data, mpack_node_str(currentNode_));
	}

	template<>
	inline void BinaryReader::TransferPrimitive<unsigned char>(unsigned char& data)
	{
		data = mpack_node_u8(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<short>(short& data)
	{
		data = mpack_node_i16(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<unsigned short>(unsigned short& data)
	{
		data = mpack_node_u16(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<int>(int& data)
	{
		data = mpack_node_i32(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<unsigned int>(unsigned int& data)
	{
		data = mpack_node_u32(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<float>(float& data)
	{
		data = mpack_node_float(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferPrimitive<double>(double& data)
	{
		data = mpack_node_double(currentNode_);
	}
}
