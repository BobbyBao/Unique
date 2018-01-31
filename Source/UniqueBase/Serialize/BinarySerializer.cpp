#include "Precompiled.h"
#include "BinarySerializer.h"
#include "../Serialize/mpack/mpack.h"
#include <fstream>

namespace Unique
{
	BinarySerializer::BinarySerializer() : Visitor(VisitState::Writing)
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
	
	void BinarySerializer::VisitBin(ByteArray& data)
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

	void BinarySerializer::VisitPrimitive(std::string& data)
	{
		mpack_write_str(&writer_, data.c_str(), (uint)data.size());
	}

	void BinarySerializer::VisitPrimitive(String& data)
	{
		mpack_write_str(&writer_, data.CString(), data.Length());
	}

	void BinarySerializer::VisitPrimitive(bool& data)
	{
		mpack_write_bool(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(char& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(unsigned char& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(short& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(unsigned short& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(int& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(unsigned int& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(long long& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(unsigned long long& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(float& data)
	{
		mpack_write(&writer_, data);
	}

	void BinarySerializer::VisitPrimitive(double& data)
	{
		mpack_write(&writer_, data);
	}
	
	void BinarySerializer::VisitPrimitive(Vector2& data)
	{
		mpack_write_bin(&writer_, (const char*)data.Data(), sizeof(Vector2));
	}
	
	void BinarySerializer::VisitPrimitive(Vector3& data)
	{
		mpack_write_bin(&writer_, (const char*)data.Data(), sizeof(Vector3));
	}
	
	void BinarySerializer::VisitPrimitive(Vector4& data)
	{
		mpack_write_bin(&writer_, (const char*)data.Data(), sizeof(Vector4));
	}
	
	void BinarySerializer::VisitPrimitive(Color& data)
	{
		mpack_write_bin(&writer_, (const char*)data.Data(), sizeof(Color));
	}
	
	void BinarySerializer::VisitPrimitive(Quaternion& data)
	{
		mpack_write_bin(&writer_, (const char*)data.Data(), sizeof(Quaternion));
	}
}