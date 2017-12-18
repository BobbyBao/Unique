#include "Precompiled.h"
#include "BinaryDeserializer.h"


namespace Unique
{
	BinaryDeserializer::BinaryDeserializer() : Serializer(TransferState::Reading)
	{
	}

	BinaryDeserializer::~BinaryDeserializer()
	{
	}

	bool BinaryDeserializer::StartObject(uint size)
	{
		return true;
	}

	void BinaryDeserializer::EndObject()
	{
	}

	void BinaryDeserializer::TransferBin(ByteArray& data)
	{
		uint size = mpack_node_data_len(currentNode_);

		data.resize(size);

		if (size != mpack_node_copy_data(currentNode_, data.data(), size))
		{
			assert(false);
		}
	}

	bool BinaryDeserializer::StartDocument(const String& fileName)
	{
		mpack_tree_init(&tree_, data_.data(), data_.size());
		mpack_tree_parse(&tree_);

		if (mpack_tree_error(&tree_) != mpack_ok)
		{
			UNIQUE_LOGERROR("Could not parse MsgPack data from " + fileName);
			return false;
		}

		currentNode_ = mpack_tree_root(&tree_);
		return true;
	}

	void BinaryDeserializer::EndDocument()
	{
		mpack_tree_destroy(&tree_);
	}

	SPtr<Object> BinaryDeserializer::CreateObject()
	{
		mpack_node_t node = mpack_node_map_cstr(currentNode_, "Type");
		if (mpack_node_type(node) == mpack_type_nil)
		{
			return nullptr;
		}

		const char* type = mpack_node_str(node);
		if (!type)
		{
			return nullptr;
		}

		size_t sz = mpack_node_strlen(node);

		return GetContext()->CreateObject(String(type, (uint)sz));
	}

	bool BinaryDeserializer::StartAttribute(const String& key)
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

	void BinaryDeserializer::EndAttribute()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}

	bool BinaryDeserializer::StartArray(uint& size)
	{
		if (mpack_node_type(currentNode_) != mpack_type_array)
		{
			assert(false);
			return false;
		}

		size = (uint)mpack_node_array_length(currentNode_);
		parentNode_.push_back(currentNode_);
		return true;
	}

	void BinaryDeserializer::SetElement(uint index)
	{
		mpack_node_t parentNode = parentNode_.back();
		currentNode_ = mpack_node_array_at(parentNode, index);
	}

	void BinaryDeserializer::EndArray()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}
	
	void BinaryDeserializer::TransferPrimitive(std::string& data)
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

	void BinaryDeserializer::TransferPrimitive(String& data)
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

	void BinaryDeserializer::TransferPrimitive(bool& data)
	{
		data = mpack_node_bool(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(char& data)
	{
		data = mpack_node_i8(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(unsigned char& data)
	{
		data = mpack_node_u8(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(short& data)
	{
		data = mpack_node_i16(currentNode_);
	}
	
	void BinaryDeserializer::TransferPrimitive(unsigned short& data)
	{
		data = mpack_node_u16(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(int& data)
	{
		data = mpack_node_i32(currentNode_);
	}
	
	void BinaryDeserializer::TransferPrimitive(unsigned int& data)
	{
		data = mpack_node_u32(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(long long& data)
	{
		data = mpack_node_i64(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(unsigned long long& data)
	{
		data = mpack_node_u64(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(float& data)
	{
		data = mpack_node_float(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(double& data)
	{
		data = mpack_node_double(currentNode_);
	}

	void BinaryDeserializer::TransferPrimitive(Vector2& data)
	{
		TransferBin((void*)data.Data(), sizeof(Vector2));
	}
	
	void BinaryDeserializer::TransferPrimitive(Vector3& data)
	{
		TransferBin((void*)data.Data(), sizeof(Vector3));
	}
	
	void BinaryDeserializer::TransferPrimitive(Vector4& data)
	{
		TransferBin((void*)data.Data(), sizeof(Vector4));
	}
	
	void BinaryDeserializer::TransferPrimitive(Color& data)
	{
		TransferBin((void*)data.Data(), sizeof(Color));
	}
	
	void BinaryDeserializer::TransferPrimitive(Quaternion& data)
	{
		TransferBin((void*)data.Data(), sizeof(Quaternion));
	}

	void BinaryDeserializer::TransferBin(void* data, size_t byteSize)
	{
		uint size = mpack_node_data_len(currentNode_);
		assert(size == byteSize);
		mpack_node_copy_data(currentNode_, (char*)data, size);
	}
}
