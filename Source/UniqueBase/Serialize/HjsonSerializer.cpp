#include "Precompiled.h"
#include "HjsonSerializer.h"



namespace Unique
{
	HjsonSerializer::HjsonSerializer() : Serializer(TransferState::Writing)
	{
	}

	HjsonSerializer::~HjsonSerializer()
	{
	}
	
	bool HjsonSerializer::StartDocument(const String& fileName)
	{
		return true;
	}
	
	void HjsonSerializer::EndDocument()
	{
	}

	bool HjsonSerializer::StartObject(uint size)
	{
		return true;
	}

	void HjsonSerializer::EndObject()
	{
	}
	
	void HjsonSerializer::TransferBin(ByteArray& data)
	{
	//	ByteArray bytes = std::move(ToBase64(data));
	//	writer_->String(bytes.data(), (rapidjson::SizeType)bytes.size());
	}

	bool HjsonSerializer::StartAttribute(const String& key)
	{
	//	writer_->Key(key.CString());
		return true;
	}

	void HjsonSerializer::EndAttribute()
	{
	}

	bool HjsonSerializer::StartArray(uint& size)
	{
	//	writer_->StartArray();
		return true;
	}

	void HjsonSerializer::SetElement(uint index)
	{
	}

	void HjsonSerializer::EndArray()
	{
	//	writer_->EndArray();
	}

	void HjsonSerializer::TransferPrimitive(std::string& data)
	{
	//	writer_->String(data.c_str());
	}

	void HjsonSerializer::TransferPrimitive(String& data)
	{
	//	writer_->String(data.CString());
	}

	void HjsonSerializer::TransferPrimitive(bool& data)
	{
	//	writer_->Bool(data);
	}

	void HjsonSerializer::TransferPrimitive(char& data)
	{
	//	writer_->Int(data);
	}

	void HjsonSerializer::TransferPrimitive(unsigned char& data)
	{
	//	writer_->Uint(data);
	}

	void HjsonSerializer::TransferPrimitive(short& data)
	{
	//	writer_->Int(data);
	}

	void HjsonSerializer::TransferPrimitive(unsigned short& data)
	{
	//	writer_->Uint(data);
	}

	void HjsonSerializer::TransferPrimitive(int& data)
	{
	//	writer_->Int(data);
	}

	void HjsonSerializer::TransferPrimitive(unsigned int& data)
	{
	//	writer_->Uint(data);
	}

	void HjsonSerializer::TransferPrimitive(long long& data)
	{
	//	writer_->Int64(data);
	}

	void HjsonSerializer::TransferPrimitive(unsigned long long& data)
	{
	//	writer_->Uint64(data);
	}

	void HjsonSerializer::TransferPrimitive(float& data)
	{
	//	writer_->Double(data);
	}

	void HjsonSerializer::TransferPrimitive(double& data)
	{
	//	writer_->Double(data);
	}
}