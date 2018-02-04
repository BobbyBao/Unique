#pragma once
#include "../Visitor.h"
#include "hjson/hjson.h"

namespace Hjson
{
	class Value;
}

namespace Unique
{
	class HjsonSerializer : public Serializer
	{
	public:
		HjsonSerializer();
		~HjsonSerializer();

		virtual bool StartDocument(const String& fileName);
		virtual void EndDocument();
		virtual bool StartObject(uint size);
		virtual void EndObject();
		virtual bool StartAttribute(const String& key);
		virtual void EndAttribute();
		virtual bool StartArray(uint& size);
		virtual void SetElement(uint index);
		virtual void EndArray();
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
	private:
		Hjson::Value currentNode_;
	};

}


