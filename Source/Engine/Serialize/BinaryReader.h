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
	protected:
		bool StartProperty(const String& key);
		void EndProperty();
		bool StartArray(uint size) { return true; }
		void EndArray() {}

		mpack_tree_t tree_;
		mpack_node_t currentNode_;
		mpack_node_t parentNode_;
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



	inline bool BinaryReader::StartProperty(const String& key)
	{
		mpack_node_t node = mpack_node_map_str(currentNode_, key.CString(), key.Length());
		if (mpack_node_type(node) == mpack_type_nil)
		{
			return false;
		}

		parentNode_ = currentNode_;
		currentNode_ = node;
		return true; 
	}

	inline void BinaryReader::EndProperty()
	{
		currentNode_ = parentNode_;
		parentNode_ = { nullptr, nullptr };
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

			data = StaticCast<T, Object>(Object::GetContext()->CreateObject(type));
		}

		data->Transfer(*this);
	}

	template<class T>
	inline void BinaryReader::TransferSTLStyleArray(T& data, int metaFlag)
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
	inline void BinaryReader::TransferSTLStyleMap(T& data, int metaFlag)
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

		for (SizeType i = 0; i < parentNode->Size(); ++i)
		{
			currentNode_ = mpack_node_array_at(parentNode, i);
			non_const_value_type val;
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
			data.insert(val);
		}

		currentNode_ = parentNode;
	}

	template<class T>
	inline void BinaryReader::TransferSTLStyleSet(T& data, int metaFlag)
	{
		if (mpack_node_type(currentNode_) != mpack_type_array)
		{
			return;
		}

		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		mpack_node_t parentNode = currentNode_;
		
		for (SizeType i = 0; i < parentNode->Size(); ++i)
		{
			currentNode_ = mpack_node_array_at(parentNode, i);
			non_const_value_type val;
			SerializeTraits<non_const_value_type>::Transfer(val, *this);
			data.insert(val);
		}

		currentNode_ = parentNode;
	}

	template<class T>
	inline void BinaryReader::TransferBasicData(T& data)
	{
		data.Transfer(*this);
	}

	template<>
	inline void BinaryReader::TransferBasicData<bool>(bool& data)
	{
		data = mpack_node_bool(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<char>(char& data)
	{
		data = mpack_node_i8(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<String>(String& data)
	{
		const char* str = mpack_node_str(currentNode_);
		if (str)
		{
			size_t sz = mpack_node_strlen(currentNode_);
			data = String(str, (uint)sz);
		}
		else
		{
			data.clear();
		}
	}

	template<>
	inline void BinaryReader::TransferBasicData<std::string>(std::string& data)
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
	inline void BinaryReader::TransferBasicData<char*>(char*& data)
	{
		std::strcpy(data, mpack_node_str(currentNode_));
	}

	template<>
	inline void BinaryReader::TransferBasicData<unsigned char>(unsigned char& data)
	{
		data = mpack_node_u8(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<short>(short& data)
	{
		data = mpack_node_i16(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<unsigned short>(unsigned short& data)
	{
		data = mpack_node_u16(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<int>(int& data)
	{
		data = mpack_node_i32(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<unsigned int>(unsigned int& data)
	{
		data = mpack_node_u32(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<float>(float& data)
	{
		data = mpack_node_float(currentNode_);
	}

	template<>
	inline void BinaryReader::TransferBasicData<double>(double& data)
	{
		data = mpack_node_double(currentNode_);
	}
}
