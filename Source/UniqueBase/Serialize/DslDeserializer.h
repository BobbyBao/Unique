#pragma once
#include "Serializer.h"
#include "Serialize/DSL/AstParser.h"

namespace Unique
{

	class DslDeserializer : public Serializer
	{
	public:
		DslDeserializer();
		~DslDeserializer();
		virtual bool StartObject(uint size);
		virtual void EndObject();
		virtual void TransferBin(ByteArray& data);
	protected:
		virtual bool StartDocument(const String& fileName);
		virtual void EndDocument();
		virtual SPtr<Object> CreateObject();
		virtual bool StartAttribute(const String& key);
		virtual void EndAttribute();
		virtual bool StartArray(uint& size);
		virtual void SetElement(uint index);
		virtual void EndArray();

		virtual void TransferPrimitive(std::string& data);
		virtual void TransferPrimitive(String& data);
		virtual void TransferPrimitive(bool& data);
		virtual void TransferPrimitive(char& data);
		virtual void TransferPrimitive(unsigned char& data);
		virtual void TransferPrimitive(short& data);
		virtual void TransferPrimitive(unsigned short& data);
		virtual void TransferPrimitive(int& data);
		virtual void TransferPrimitive(unsigned int& data);
		virtual void TransferPrimitive(long long& data);
		virtual void TransferPrimitive(unsigned long long& data);
		virtual void TransferPrimitive(float& data);
		virtual void TransferPrimitive(double& data);
		virtual void TransferPrimitive(Vector2& data);
		virtual void TransferPrimitive(Vector3& data);
		virtual void TransferPrimitive(Vector4& data);
		virtual void TransferPrimitive(Color& data);
		virtual void TransferPrimitive(Quaternion& data);
	private:
		AstParser parser_;
		AstNode nameNode_;
		AstNode* currentNode_ = nullptr;
		Vector<AstNode*> parentNode_;
		ChildMap::_Pairii childIterator_;
		ChildMap::iterator currentPos_;
		size_t arraySize_ = 0;
	};

}

