#include "Precompiled.h"
#include "HjsonDeserializer.h"
#include "rapidjson/rapidjson.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>

namespace Unique
{
	HjsonDeserializer::HjsonDeserializer(bool dsl) 
		: Serializer(TransferState::Reading), dsl_(dsl)
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
	//	const char* base64 = currentNode_->GetString();
	//	data = FromBase64(base64, currentNode_->GetStringLength());
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
				hJsonNode_ = it->second;
				
				if (rootObject_)
				{
					std::string name;
					if (SplitTypeInfo(key.c_str(), rootType_, name))
					{
						hJsonNode_["Type"] = rootType_;
						hJsonNode_["Name"] = name;
					}

				}

			}
		}
		else
		{
			hJsonRoot = Hjson::Unmarshal(data_.data(), data_.size());
			hJsonNode_ = hJsonRoot;
		}

		return true;
	}

	void HjsonDeserializer::EndDocument()
	{
	}

	SPtr<Object> HjsonDeserializer::CreateObject()
	{
		Hjson::Value v = hJsonNode_["Type"];
		if (v.empty())
		{
			UNIQUE_LOGWARNING("Unkown object type.");
			return nullptr;
		}
		
		return GetContext()->CreateObject((const char*)v);
	}

	bool HjsonDeserializer::StartAttribute(const String& key)
	{
		if (dsl_ && ((metaFlag_ & AttributeFlag::Vector) || (metaFlag_ & AttributeFlag::Map)))
		{
			Vector<std::pair<std::string, Hjson::Value>> elements;
			for (auto& it = hJsonNode_.begin(); it != hJsonNode_.end(); ++it)
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
					hJsonNode_.erase(e.first);
				}

				hJsonNode_[key.CString()] = v;
				hJsonParentNode_.push_back(hJsonNode_);
				hJsonNode_ = v;
			}
		}
		else
		{
			Hjson::Value v = hJsonNode_[key.CString()];
			if (v.empty())
			{
				return false;
			}

			hJsonParentNode_.push_back(hJsonNode_);
			hJsonNode_ = v;
		}

		return true;
	}

	void HjsonDeserializer::EndAttribute()
	{
		hJsonNode_ = hJsonParentNode_.back();
		hJsonParentNode_.pop_back();
	}

	bool HjsonDeserializer::StartArray(uint& size)
	{
		if (hJsonNode_.type() != Hjson::Value::VECTOR)
		{
			assert(false);
			return false;
		}

		size = (uint)hJsonNode_.size();
		hJsonParentNode_.push_back(hJsonNode_);
		return true;
	}

	void HjsonDeserializer::SetElement(uint index)
	{
		auto parentNode = hJsonParentNode_.back();
		auto& child = parentNode[index];
		hJsonNode_ = child;
	}

	void HjsonDeserializer::EndArray()
	{
		hJsonNode_ = hJsonParentNode_.back();
		hJsonParentNode_.pop_back();
	}
	
	void HjsonDeserializer::TransferPrimitive(std::string& data)
	{
		data = (const char*)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(String& data)
	{
		data = (const char*)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(bool& data)
	{
		data = (bool)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(char& data)
	{
		data = (char)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(unsigned char& data)
	{
		data = (byte)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(short& data)
	{
		data = (short)hJsonNode_;
	}
	
	void HjsonDeserializer::TransferPrimitive(unsigned short& data)
	{
		data = (ushort)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(int& data)
	{
		data = (int)hJsonNode_;
	}
	
	void HjsonDeserializer::TransferPrimitive(unsigned int& data)
	{
		data = (uint)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(long long& data)
	{
	//	data = (uint)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(unsigned long long& data)
	{
	//	assert(currentNode_->IsUint64());
	//	data = currentNode_->GetUint64();
	}

	void HjsonDeserializer::TransferPrimitive(float& data)
	{
		data = (float)hJsonNode_;
	}

	void HjsonDeserializer::TransferPrimitive(double& data)
	{
		data = (double)hJsonNode_;
	}
}
