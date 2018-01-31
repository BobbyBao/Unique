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
	JsonSerializer::JsonSerializer() : Visitor(VisitState::Writing)
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
	
	void JsonSerializer::VisitBin(ByteArray& data)
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

	void JsonSerializer::VisitPrimitive(std::string& data)
	{
		writer_->String(data.c_str());
	}

	void JsonSerializer::VisitPrimitive(String& data)
	{
		writer_->String(data.CString());
	}

	void JsonSerializer::VisitPrimitive(bool& data)
	{
		writer_->Bool(data);
	}

	void JsonSerializer::VisitPrimitive(char& data)
	{
		writer_->Int(data);
	}

	void JsonSerializer::VisitPrimitive(unsigned char& data)
	{
		writer_->Uint(data);
	}

	void JsonSerializer::VisitPrimitive(short& data)
	{
		writer_->Int(data);
	}

	void JsonSerializer::VisitPrimitive(unsigned short& data)
	{
		writer_->Uint(data);
	}

	void JsonSerializer::VisitPrimitive(int& data)
	{
		writer_->Int(data);
	}

	void JsonSerializer::VisitPrimitive(unsigned int& data)
	{
		writer_->Uint(data);
	}

	void JsonSerializer::VisitPrimitive(long long& data)
	{
		writer_->Int64(data);
	}

	void JsonSerializer::VisitPrimitive(unsigned long long& data)
	{
		writer_->Uint64(data);
	}

	void JsonSerializer::VisitPrimitive(float& data)
	{
		writer_->Double(data);
	}

	void JsonSerializer::VisitPrimitive(double& data)
	{
		writer_->Double(data);
	}
	
	void JsonSerializer::VisitPrimitive(Vector2& data)
	{
		writer_->StartArray();
		VisitPrimitive(data.x_);
		VisitPrimitive(data.y_);
		writer_->EndArray();
	}
	
	void JsonSerializer::VisitPrimitive(Vector3& data)
	{
		writer_->StartArray();
		VisitPrimitive(data.x_);
		VisitPrimitive(data.y_);
		VisitPrimitive(data.z_);
		writer_->EndArray();
	}
	
	void JsonSerializer::VisitPrimitive(Vector4& data)
	{
		writer_->StartArray();
		VisitPrimitive(data.x_);
		VisitPrimitive(data.y_);
		VisitPrimitive(data.z_);
		VisitPrimitive(data.w_);
		writer_->EndArray();
	}
	
	void JsonSerializer::VisitPrimitive(Color& data)
	{
		writer_->StartArray();
		VisitPrimitive(data.r_);
		VisitPrimitive(data.g_);
		VisitPrimitive(data.b_);
		VisitPrimitive(data.a_);
		writer_->EndArray();
	}
	
	void JsonSerializer::VisitPrimitive(Quaternion& data)
	{
		writer_->StartArray();
		VisitPrimitive(data.w_);
		VisitPrimitive(data.x_);
		VisitPrimitive(data.y_);
		VisitPrimitive(data.z_);
		writer_->EndArray();
	}
}