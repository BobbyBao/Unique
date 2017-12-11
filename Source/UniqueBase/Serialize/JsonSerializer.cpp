#include "Precompiled.h"
#include "JsonSerializer.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>

using namespace rapidjson;

namespace Unique
{
	JsonSerializer::JsonSerializer() : Serializer(TransferState::Writing)
	{
	}

	JsonSerializer::~JsonSerializer()
	{
		if (ostreamWrapper)
		{
			delete ostreamWrapper;
		}

		if (writer_)
		{
			delete writer_;
		}
	}
	
	bool JsonSerializer::StartDocument(const String& fileName)
	{
		jsonFile.open(fileName.CString());
		if (!jsonFile.is_open())
		{
			return false;
		}

		ostreamWrapper = new OStreamWrapper(jsonFile);
		writer_ = new PrettyWriter<OStreamWrapper>(*ostreamWrapper);
		return true;
	}
	
	void JsonSerializer::EndDocument()
	{
	}

	bool JsonSerializer::StartObject(uint size)
	{
		writer_->StartObject();
		return true;
	}

	void JsonSerializer::EndObject()
	{
		writer_->EndObject();
	}
	
	void JsonSerializer::TransferBin(ByteArray& data)
	{
		ByteArray bytes = std::move(ToBase64(data));
		writer_->String(bytes.data(), (rapidjson::SizeType)bytes.size());
	}

	bool JsonSerializer::StartAttribute(const String& key)
	{
		writer_->Key(key.CString());
		return true;
	}

	void JsonSerializer::EndAttribute()
	{
	}

	bool JsonSerializer::StartArray(uint& size)
	{
		writer_->StartArray();
		return true;
	}

	void JsonSerializer::SetElement(uint index)
	{
	}

	void JsonSerializer::EndArray()
	{
		writer_->EndArray();
	}

	void JsonSerializer::TransferPrimitive(std::string& data)
	{
		writer_->String(data.c_str());
	}

	void JsonSerializer::TransferPrimitive(String& data)
	{
		writer_->String(data.CString());
	}

	void JsonSerializer::TransferPrimitive(bool& data)
	{
		writer_->Bool(data);
	}

	void JsonSerializer::TransferPrimitive(char& data)
	{
		writer_->Int(data);
	}

	void JsonSerializer::TransferPrimitive(unsigned char& data)
	{
		writer_->Uint(data);
	}

	void JsonSerializer::TransferPrimitive(short& data)
	{
		writer_->Int(data);
	}

	void JsonSerializer::TransferPrimitive(unsigned short& data)
	{
		writer_->Uint(data);
	}

	void JsonSerializer::TransferPrimitive(int& data)
	{
		writer_->Int(data);
	}

	void JsonSerializer::TransferPrimitive(unsigned int& data)
	{
		writer_->Uint(data);
	}

	void JsonSerializer::TransferPrimitive(long long& data)
	{
		writer_->Int64(data);
	}

	void JsonSerializer::TransferPrimitive(unsigned long long& data)
	{
		writer_->Uint64(data);
	}

	void JsonSerializer::TransferPrimitive(float& data)
	{
		writer_->Double(data);
	}

	void JsonSerializer::TransferPrimitive(double& data)
	{
		writer_->Double(data);
	}
}