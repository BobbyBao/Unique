#include "Precompiled.h"
#include "HjsonSerializer.h"



namespace Unique
{
	HjsonSerializer::HjsonSerializer() : Visitor(VisitState::Writing)
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
	
	void HjsonSerializer::VisitBin(ByteArray& data)
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

	void HjsonSerializer::VisitPrimitive(std::string& data)
	{
	//	writer_->String(data.c_str());
	}

	void HjsonSerializer::VisitPrimitive(String& data)
	{
	//	writer_->String(data.CString());
	}

	void HjsonSerializer::VisitPrimitive(bool& data)
	{
	//	writer_->Bool(data);
	}

	void HjsonSerializer::VisitPrimitive(char& data)
	{
	//	writer_->Int(data);
	}

	void HjsonSerializer::VisitPrimitive(unsigned char& data)
	{
	//	writer_->Uint(data);
	}

	void HjsonSerializer::VisitPrimitive(short& data)
	{
	//	writer_->Int(data);
	}

	void HjsonSerializer::VisitPrimitive(unsigned short& data)
	{
	//	writer_->Uint(data);
	}

	void HjsonSerializer::VisitPrimitive(int& data)
	{
	//	writer_->Int(data);
	}

	void HjsonSerializer::VisitPrimitive(unsigned int& data)
	{
	//	writer_->Uint(data);
	}

	void HjsonSerializer::VisitPrimitive(long long& data)
	{
	//	writer_->Int64(data);
	}

	void HjsonSerializer::VisitPrimitive(unsigned long long& data)
	{
	//	writer_->Uint64(data);
	}

	void HjsonSerializer::VisitPrimitive(float& data)
	{
	//	writer_->Double(data);
	}

	void HjsonSerializer::VisitPrimitive(double& data)
	{
	//	writer_->Double(data);
	}
}