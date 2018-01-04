#include "Precompiled.h"
#include "DslDeserializer.h"


namespace Unique
{
	DslDeserializer::DslDeserializer() : Visitor(TransferState::Reading), nameNode_("")
	{
	}

	DslDeserializer::~DslDeserializer()
	{
	}

	bool DslDeserializer::StartObject(uint size)
	{
		return true;
	}

	void DslDeserializer::EndObject()
	{
	}

	void DslDeserializer::TransferBin(ByteArray& data)
	{
		data = FromBase64(currentNode_->value_);
	}

	bool DslDeserializer::StartDocument(const String& fileName)
	{
		if (!parser_.Parse(data_.data(), data_.size()))
		{
			return false;
		}

		//parser_.Print();
 		currentNode_ = parser_.root_[0].get();
		return true;
	}

	void DslDeserializer::EndDocument()
	{
	}

	SPtr<Object> DslDeserializer::CreateObject()
	{		
		return GetContext()->CreateObject(currentNode_->token_);
	}

	bool DslDeserializer::StartAttribute(const String& key)
	{
		if (0 == key.Compare("Name", false))
		{
			nameNode_.value_ = currentNode_->value_;
			parentNode_.push_back(currentNode_);			
			currentNode_ = &nameNode_;
			return true;
		}

		AstNode* node = currentNode_->GetChild(key);
		if (node == nullptr)
		{
			return false;
		}

		bool isCollection = (attributeFlag_ & AttributeFlag::Vector) || (attributeFlag_ & AttributeFlag::Map);
		if (isCollection)
		{
			childIterator_ = currentNode_->GetIterator(key);
			arraySize_ = currentNode_->GetChildCount(key);
		}
		else
		{
			parentNode_.push_back(currentNode_);
			currentNode_ = node;
		}

		return true;
	}

	void DslDeserializer::EndAttribute()
	{
		bool isCollection = (attributeFlag_ & AttributeFlag::Vector) || (attributeFlag_ & AttributeFlag::Map);

		if (isCollection)
		{
			arraySize_ = 0;
		}
		else
		{
			currentNode_ = parentNode_.back();
			parentNode_.pop_back();
		}
	}

	bool DslDeserializer::StartArray(uint& size)
	{
		size = (uint)arraySize_;
		parentNode_.push_back(currentNode_);
		return true;
	}

	void DslDeserializer::SetElement(uint index)
	{
		if (index == 0)
		{
			currentPos_ = childIterator_.first;
		}
		else
		{
			++currentPos_;
		}

		currentNode_ = currentPos_->second.get();
	}

	void DslDeserializer::EndArray()
	{
 		currentNode_ = parentNode_.back();
 		parentNode_.pop_back();
	}
	
	void DslDeserializer::TransferPrimitive(std::string& data)
	{
		data = currentNode_->value_.CString();
	}

	void DslDeserializer::TransferPrimitive(String& data)
	{
		data = currentNode_->value_;
	}

	void DslDeserializer::TransferPrimitive(bool& data)
	{
		data = FromString<bool>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(char& data)
	{
		data = FromString<char>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(unsigned char& data)
	{
		data = FromString<unsigned char>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(short& data)
	{
		data = FromString<short>(currentNode_->value_);
	}
	
	void DslDeserializer::TransferPrimitive(unsigned short& data)
	{
		data = FromString<unsigned short>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(int& data)
	{
		data = FromString<int>(currentNode_->value_);
	}
	
	void DslDeserializer::TransferPrimitive(unsigned int& data)
	{
		data = FromString<unsigned int>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(long long& data)
	{
		data = FromString<long long>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(unsigned long long& data)
	{
		data = FromString<unsigned long long>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(float& data)
	{
		data = FromString<float>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(double& data)
	{
		data = FromString<double>(currentNode_->value_);
	}

	void DslDeserializer::TransferPrimitive(Vector2& data)
	{
		data = FromString<Vector2>(currentNode_->value_);
	}
	
	void DslDeserializer::TransferPrimitive(Vector3& data)
	{
		data = FromString<Vector3>(currentNode_->value_);
	}
	
	void DslDeserializer::TransferPrimitive(Vector4& data)
	{
		data = FromString<Vector4>(currentNode_->value_);
	}
	
	void DslDeserializer::TransferPrimitive(Color& data)
	{
		data = FromString<Color>(currentNode_->value_);
	}
	
	void DslDeserializer::TransferPrimitive(Quaternion& data)
	{
		data = FromString<Quaternion>(currentNode_->value_);
	}

}
