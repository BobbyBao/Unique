#pragma once
#include "Visitor.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

using namespace rapidjson;

namespace Unique
{

	class JsonDeserializer : public Visitor
	{
	public:
		JsonDeserializer();
		~JsonDeserializer();
		virtual bool StartObject(uint size);
		virtual void EndObject();
		virtual void VisitBin(ByteArray& data);

		virtual void VisitPrimitive(std::string& data);
		virtual void VisitPrimitive(String& data);
		virtual void VisitPrimitive(bool& data);
		virtual void VisitPrimitive(char& data);
		virtual void VisitPrimitive(unsigned char& data);
		virtual void VisitPrimitive(short& data);
		virtual void VisitPrimitive(unsigned short& data);
		virtual void VisitPrimitive(int& data);
		virtual void VisitPrimitive(unsigned int& data);
		virtual void VisitPrimitive(long long& data);
		virtual void VisitPrimitive(unsigned long long& data);
		virtual void VisitPrimitive(float& data);
		virtual void VisitPrimitive(double& data);
		virtual void VisitPrimitive(Vector2& data);
		virtual void VisitPrimitive(Vector3& data);
		virtual void VisitPrimitive(Vector4& data);
		virtual void VisitPrimitive(Color& data);
		virtual void VisitPrimitive(Quaternion& data);
	protected:
		virtual bool StartDocument(const String& fileName);
		virtual void EndDocument();
		virtual SPtr<Object> CreateObject();
		virtual bool StartAttribute(const String& key);
		virtual void EndAttribute();
		virtual bool StartArray(uint& size);
		virtual void SetElement(uint index);
		virtual void EndArray();

	private:
		UPtr<rapidjson::Document> document = nullptr;
		rapidjson::Value* currentNode_ = nullptr;
		Vector<rapidjson::Value*> parentNode_;
	};

}

