#include "Precompiled.h"
#include "HjsonDeserializer.h"


namespace Unique
{
	HjsonDeserializer::HjsonDeserializer(bool dsl) 
		: Visitor(TransferState::Reading), dsl_(dsl)
	{
	}

	HjsonDeserializer::~HjsonDeserializer()
	{
	}

	bool HjsonDeserializer::StartObject(uint size)
	{
		return true;
	}

	void HjsonDeserializer::EndObject()
	{
	}

	void HjsonDeserializer::TransferBin(ByteArray& data)
	{
		const std::string& base64 = (const std::string&)currentNode_;
		data = FromBase64(base64.c_str(), base64.length());
	}

	inline bool SplitTypeInfo(const std::string& info, const std::string& type, std::string& name)
	{
		if (std::strncmp(info.c_str(), type.c_str(), type.length()) != 0)
		{
			return false;
		}

		if (info.length() == type.length())
		{
			return true;
		}

		size_t pos = type.length();
		if (info[pos] != '(')
		{
			return false;
		}

		size_t pos1 = info.find_last_of(')');
		if (pos1 != std::string::npos)
		{
			pos++;
			pos1--;
			if (info[pos] == '"')
			{
				pos++;
			}

			if (info[pos1] == '"')
			{
				pos1--;
			}

			if (pos1 > pos)
			{
				name = info.substr(pos, pos1 - pos + 1);
			}
		}

		return true;
	}

	bool HjsonDeserializer::StartDocument(const String& fileName)
	{
		Hjson::Value hJsonRoot;
	
		if (dsl_)
		{
			hJsonRoot = Hjson::Unmarshal(data_.data(), data_.size());
			for (auto& it = hJsonRoot.begin(); it != hJsonRoot.end(); ++it)
			{
				auto& key = it->first;
				currentNode_ = it->second;
				
				if (rootObject_)
				{
					std::string name;
					if (SplitTypeInfo(key.c_str(), rootType_, name))
					{
						currentNode_["Type"] = rootType_;
						currentNode_["Name"] = name;
					}

				}

			}
		}
		else
		{
			hJsonRoot = Hjson::Unmarshal(data_.data(), data_.size());
			currentNode_ = hJsonRoot;
		}

		return true;
	}

	void HjsonDeserializer::EndDocument()
	{
	}

	SPtr<Object> HjsonDeserializer::CreateObject()
	{
		Hjson::Value v = currentNode_["Type"];
		if (v.empty())
		{
			UNIQUE_LOGWARNING("Unkown object type.");
			return nullptr;
		}
		
		return GetContext()->CreateObject((const char*)v);
	}

	bool HjsonDeserializer::StartAttribute(const String& key)
	{
		if (dsl_ && ((attributeFlag_ & AttributeFlag::Vector) || (attributeFlag_ & AttributeFlag::Map)))
		{
			Vector<std::pair<std::string, Hjson::Value>> elements;
			for (auto& it = currentNode_.begin(); it != currentNode_.end(); ++it)
			{
				auto& mapKey = it->first;
				auto& mapValue = it->second;
				std::string name;
				if (SplitTypeInfo(mapKey, key.CString(), name))
				{
					elements.push_back(*it);

					if (it->second.type() == Hjson::Value::MAP)
					{
						if (!name.empty())
						{
							mapValue["Name"] = name.c_str();
						}

						if (mapValue["Type"].empty())
						{
							mapValue["Type"] = key.CString();
						}
					}

				}
			}

			if (elements.empty())
			{
				return false;
			}
			else
			{
				Hjson::Value v(Hjson::Value::VECTOR);
				for (auto& e : elements)
				{
					v.push_back(e.second);
					currentNode_.erase(e.first);
				}

				currentNode_[key.CString()] = v;
				parentNode_.push_back(currentNode_);
				currentNode_ = v;
			}
		}
		else
		{
			Hjson::Value v = currentNode_[key.CString()];
			if (v.empty())
			{
				return false;
			}

			parentNode_.push_back(currentNode_);
			currentNode_ = v;
		}

		return true;
	}

	void HjsonDeserializer::EndAttribute()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}

	bool HjsonDeserializer::StartArray(uint& size)
	{
		if (currentNode_.type() != Hjson::Value::VECTOR)
		{
			assert(false);
			return false;
		}

		size = (uint)currentNode_.size();
		parentNode_.push_back(currentNode_);
		return true;
	}

	void HjsonDeserializer::SetElement(uint index)
	{
		auto parentNode = parentNode_.back();
		auto& child = parentNode[index];
		currentNode_ = child;
	}

	void HjsonDeserializer::EndArray()
	{
		currentNode_ = parentNode_.back();
		parentNode_.pop_back();
	}
	
	void HjsonDeserializer::TransferPrimitive(std::string& data)
	{
		data = (const char*)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(String& data)
	{
		data = (const char*)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(bool& data)
	{
		data = (bool)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(char& data)
	{
		data = (char)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(unsigned char& data)
	{
		data = (byte)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(short& data)
	{
		data = (short)currentNode_;
	}
	
	void HjsonDeserializer::TransferPrimitive(unsigned short& data)
	{
		data = (ushort)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(int& data)
	{
		data = (int)currentNode_;
	}
	
	void HjsonDeserializer::TransferPrimitive(unsigned int& data)
	{
		data = (uint)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(long long& data)
	{
	//	data = (uint)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(unsigned long long& data)
	{
	//	assert(currentNode_->IsUint64());
	//	data = currentNode_->GetUint64();
	}

	void HjsonDeserializer::TransferPrimitive(float& data)
	{
		data = (float)currentNode_;
	}

	void HjsonDeserializer::TransferPrimitive(double& data)
	{
		data = (double)currentNode_;
	}
}
