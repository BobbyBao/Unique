#include "Precompiled.h"
#include "BinarySerializer.h"
#include "../Serialize/mpack/mpack.h"
#include <fstream>

namespace Unique
{
	BinarySerializer::BinarySerializer() : Serializer(TransferState::Writing)
	{
	}

	BinarySerializer::~BinarySerializer()
	{
	}
	
	bool BinarySerializer::StartDocument(const String& fileName)
	{
		fileName_ = fileName;
		mpack_writer_init_growable(&writer_, &buffer_, &buffSize_);
		return true;
	}
	
	void BinarySerializer::EndDocument()
	{	
		// finish writing
		if (mpack_writer_destroy(&writer_) != mpack_ok)
		{
			UNIQUE_LOGERROR("An error occurred encoding the data!");
			return;
		}

		std::ofstream packFile(fileName_);
		packFile.write(buffer_, buffSize_);
	}

	bool BinarySerializer::StartObject(uint size)
	{
		mpack_start_map(&writer_, (uint)buffSize_);
		return true;
	}

	void BinarySerializer::EndObject()
	{
		mpack_finish_map(&writer_);
	}
	
	void BinarySerializer::TransferBin(ByteArray& data)
	{
		mpack_write_bin(&writer_, data.data(), (uint)data.size());
	}

	bool BinarySerializer::StartAttribute(const String& key)
	{
		mpack_write_str(&writer_, key.CString(), (uint)key.Length());
		return true;
	}

	void BinarySerializer::EndAttribute()
	{
	}

	bool BinarySerializer::StartArray(uint& size)
	{
		mpack_start_array(&writer_, size);
		return true;
	}

	void BinarySerializer::SetElement(uint index)
	{
	}

	void BinarySerializer::EndArray()
	{
		mpack_finish_array(&writer_);
	}

	void BinarySerializer::TransferPrimitive(std::string& data)
	{
		mpack_write_str(&writer_, data.c_str(), (uint)data.size());
	}

	void BinarySerializer::TransferPrimitive(String& data)
	{
		mpack_write_str(&writer_, data.CString(), data.Length());
	}

	void BinarySerializer::TransferPrimitive(bool& data)
	{
		mpack_write_bool(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(char& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(unsigned char& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(short& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(unsigned short& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(int& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(unsigned int& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(long long& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(unsigned long long& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(float& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::TransferPrimitive(double& data)
	{
		mpack_write(&writer_, data);
	}
}