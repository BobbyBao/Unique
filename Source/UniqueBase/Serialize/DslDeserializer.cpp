#include "Precompiled.h"
#include "DslDeserializer.h"


namespace Unique
{
	DslDeserializer::DslDeserializer() : Visitor(VisitState::Reading), nameNode_("")
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

	void DslDeserializer::VisitBin(ByteArray& data)
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

		bool isCollection = (attributeFlag_ & AF_VECTOR) || (attributeFlag_ & AF_MAP);
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
		bool isCollection = (attributeFlag_ & AF_VECTOR) || (attributeFlag_ & AF_MAP);

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
	
	void DslDeserializer::VisitPrimitive(std::string& data)
	{
		data = currentNode_->value_.CString();
	}

	void DslDeserializer::VisitPrimitive(String& data)
	{
		data = currentNode_->value_;
	}

	void DslDeserializer::VisitPrimitive(bool& data)
	{
		data = FromString<bool>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(char& data)
	{
		data = FromString<char>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(unsigned char& data)
	{
		data = FromString<unsigned char>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(short& data)
	{
		data = FromString<short>(currentNode_->value_);
	}
	
	void DslDeserializer::VisitPrimitive(unsigned short& data)
	{
		data = FromString<unsigned short>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(int& data)
	{
		data = FromString<int>(currentNode_->value_);
	}
	
	void DslDeserializer::VisitPrimitive(unsigned int& data)
	{
		data = FromString<unsigned int>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(long long& data)
	{
		data = FromString<long long>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(unsigned long long& data)
	{
		data = FromString<unsigned long long>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(float& data)
	{
		data = FromString<float>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(double& data)
	{
		data = FromString<double>(currentNode_->value_);
	}

	void DslDeserializer::VisitPrimitive(Vector2& data)
	{
		data = FromString<Vector2>(currentNode_->value_);
	}
	
	void DslDeserializer::VisitPrimitive(Vector3& data)
	{
		data = FromString<Vector3>(currentNode_->value_);
	}
	
	void DslDeserializer::VisitPrimitive(Vector4& data)
	{
		data = FromString<Vector4>(currentNode_->value_);
	}
	
	void DslDeserializer::VisitPrimitive(Color& data)
	{
		data = FromString<Color>(currentNode_->value_);
	}
	
	void DslDeserializer::VisitPrimitive(Quaternion& data)
	{
		data = FromString<Quaternion>(currentNode_->value_);
	}

}
