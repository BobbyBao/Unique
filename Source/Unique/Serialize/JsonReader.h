#pragma once

#include "../serialize/SerializeTraits.h"
#include "../Serialize/TransferBase.h"
#include "../IO/File.h"
#include "rapidjson/rapidjson.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
#include "hjson/hjson.h"

using namespace rapidjson;

namespace Unique
{

	class JsonReader
	{
		uSerializer(JsonReader, TransferState::Reading)
	public:
		~JsonReader() {}

		template<class T>
		bool Load(const String& fileName, T& data, bool hum = false, bool dsl = false);
		
		template<class T>
		bool Load(File& source, T& data, bool hum = false, bool dsl = false);
		
		template<class T>
		void TransferPrimitive(T& data);
		
		template<class T>
		void TransferObject(SPtr<T>& data);

		template<class T>
		void TransferArray(T& data, int metaFlag = 0);

		template<class T>
		void TransferMap(T& data, int metaFlag = 0);

		template<class T>
		void TransferSet(T& data, int metaFlag = 0);

		void TransferBin(ByteArray& data);
		
		bool StartObject(uint size) { return true; }

		void EndObject() {}
	private:
		bool StartAttribute(const String& key);
		void EndAttribute();
		bool StartArray(uint size) { return true; }
		void EndArray() {}
		bool SplitTypeInfo(const std::string& info, const std::string& type, std::string& name);

		bool human_ = false;
		bool dsl_ = false;
		Value* currentNode_ = nullptr;
		Vector<Value*> parentNode_;

		Hjson::Value hJsonNode_;
		Vector<Hjson::Value> hJsonParentNode_;
		Vector<String> currentObjectName_;
	};

	template<class T>
	inline	bool JsonReader::Load(const String& fileName, T& data, bool hum, bool dsl)
	{
		auto& cache = Subsystem<ResourceCache>();
		SPtr<File> file = cache.GetFile(fileName);
		if (!file)
		{
			return false;
		}

		return Load(*file, data, hum, dsl);
	}

	template<class T>
	inline bool JsonReader::Load(File& source, T& data, bool hum, bool dsl)
	{
		human_ = hum;
		dsl_ = dsl;

		unsigned dataSize = source.GetSize();
		if (!dataSize && !source.GetName().Empty())
		{
			UNIQUE_LOGERROR("Zero sized JSON data in " + source.GetName());
			return false;
		}

		SharedArrayPtr<char> buffer(new char[dataSize]);
		if (source.Read(buffer.Get(), dataSize) != dataSize)
			return false;
		
		rapidjson::Document document;
		Hjson::Value hJsonRoot;
		if (human_)
		{
			if (dsl)
			{
				hJsonRoot = Hjson::Unmarshal(buffer, dataSize);
				for (auto& it = hJsonRoot.begin(); it != hJsonRoot.end(); ++it)
				{
					auto& key = it->first;
					hJsonNode_ = it->second;

					if (SerializeTraits<T>::IsObject())
					{
						std::string name;
						if (SplitTypeInfo(key.c_str(), SerializeTraits<T>::GetTypeName(), name))
						{
							hJsonNode_["Type"] = SerializeTraits<T>::GetTypeName();
							hJsonNode_["Name"] = name;
						}

					}

					SerializeTraits<T>::Transfer(data, *this);
				}
			}
			else
			{
				hJsonRoot = Hjson::Unmarshal(buffer, dataSize);
				hJsonNode_ = hJsonRoot;
				SerializeTraits<T>::Transfer(data, *this);
			}
		}
		else
		{
			if (document.Parse<kParseCommentsFlag | kParseTrailingCommasFlag>(buffer, dataSize).HasParseError())
			{
				UNIQUE_LOGERROR("Could not parse JSON data from " + source.GetName());
				return false;
			}

			currentNode_ = &document;
			SerializeTraits<T>::Transfer(data, *this);
		}

		return true;
	}

	inline bool JsonReader::SplitTypeInfo(const std::string& info, const std::string& type, std::string& name)
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
		
	inline bool JsonReader::StartAttribute(const String& key)
	{
		if (human_)
		{
			if (dsl_ && ((metaFlag_ & AttributeFlag::Array) || (metaFlag_ & AttributeFlag::Map)))
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


		}
		else
		{
			Value::MemberIterator node = currentNode_->FindMember(key.CString());
			if (node == currentNode_->MemberEnd())
			{
				return false;
			}

			parentNode_.push_back(currentNode_);
			currentNode_ = &node->value;
		}
		return true;
	}

	inline void JsonReader::EndAttribute()
	{
		if (human_)
		{
			hJsonNode_ = hJsonParentNode_.back();
			hJsonParentNode_.pop_back();
		}
		else
		{
			currentNode_ = parentNode_.back();
			parentNode_.pop_back();
		}
	}

	template<class T>
	inline void JsonReader::TransferObject(SPtr<T>& data)
	{
		if (data == nullptr)
		{
			if (human_)
			{
				Hjson::Value v = hJsonNode_["Type"];
				if (v.empty())
				{
					UNIQUE_LOGWARNING("Unkown object type.");
					return;
				}

				data = StaticCast<T, Object>(GetContext()->CreateObject((const char*)v));

			}
			else
			{
				Value::MemberIterator node = currentNode_->FindMember("Type");
				if (node == currentNode_->MemberEnd())
				{
					UNIQUE_LOGWARNING("Unkown object type.");
					return;
				}

				data = StaticCast<T, Object>(GetContext()->CreateObject(node->value.GetString()));

			}
		}

		data->Transfer(*this);
	}

	template<class T>
	inline void JsonReader::TransferArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		if (human_)
		{
			if (hJsonNode_.type() != Hjson::Value::VECTOR)
			{
				assert(false);
				return;
			}

			Hjson::Value parentNode = hJsonNode_;

			for (SizeType i = 0; i < parentNode.size(); ++i)
			{
				auto& child = parentNode[i];
				hJsonNode_ = child;
				non_const_value_type val;

				SerializeTraits<non_const_value_type>::Transfer(val, *this);

				data.push_back(val);
			}

			hJsonNode_ = parentNode;

		}
		else
		{
			if (!currentNode_->IsArray())
			{
				assert(false);
				return;
			}

			Value* parentNode = currentNode_;

			for (SizeType i = 0; i < parentNode->Size(); ++i)
			{
				auto& child = (*parentNode)[i];
				currentNode_ = &child;
				non_const_value_type val;

				SerializeTraits<non_const_value_type>::Transfer(val, *this);

				data.push_back(val);
			}

			currentNode_ = parentNode;
		}

	}

	template<class T>
	inline void JsonReader::TransferMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		data.clear();

		if (human_)
		{
			if (hJsonNode_.type() != Hjson::Value::VECTOR)
			{
				assert(false);
				return;
			}

			Hjson::Value parentNode = hJsonNode_;
			for (SizeType i = 0; i < parentNode.size(); ++i)
			{
				auto& child = parentNode[i];
				hJsonNode_ = child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			hJsonNode_ = parentNode;
		}
		else
		{
			Value* parentNode = currentNode_;

			for (SizeType i = 0; i < parentNode->Size(); ++i)
			{
				auto& child = (*parentNode)[i];
				currentNode_ = &child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			currentNode_ = parentNode;
		}
	}

	template<class T>
	inline void JsonReader::TransferSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		data.clear();

		if (human_)
		{
			if (hJsonNode_.type() != Hjson::Value::VECTOR)
			{
				assert(false);
				return;
			}

			Hjson::Value parentNode = hJsonNode_;
			for (SizeType i = 0; i < parentNode.size(); ++i)
			{
				auto& child = parentNode[i];
				hJsonNode_ = child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			hJsonNode_ = parentNode;
		}
		else
		{
			if (!currentNode_->IsArray())
			{
				ASSERT(FALSE);
				return;
			}

			Value* parentNode = currentNode_;

			for (SizeType i = 0; i < parentNode->Size(); ++i)
			{
				auto& child = parentNode->operator[][i];
				currentNode_ = child;
				non_const_value_type val;
				SerializeTraits<non_const_value_type>::Transfer(val, *this);
				data.insert(val);
			}

			currentNode_ = parentNode;
		}
	}

	inline void JsonReader::TransferBin(ByteArray& data)
	{
		if (human_)
		{
			std::string& base64 = (std::string&)hJsonNode_;
			data = FromBase64(base64.c_str(), base64.length());
		}
		else
		{
			const char* base64 = currentNode_->GetString();
			data = FromBase64(base64, currentNode_->GetStringLength());
		}

	}

	template<class T>
	inline void JsonReader::TransferPrimitive(T& data)
	{
		if (human_)
		{
			data = FromString<T>(String((const char*)hJsonNode_));
		}
		else
		{
			data = FromString<T>(currentNode_->GetString());
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<String>(String& data)
	{
		if (human_)
		{
			data = (const char*)hJsonNode_;
		}
		else
		{
			data = currentNode_->GetString();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<bool>(bool& data)
	{
		if (human_)
		{
			data = (bool)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsBool());
			data = currentNode_->GetBool();
		}

	}

	template<>
	inline void JsonReader::TransferPrimitive<char>(char& data)
	{
		if (human_)
		{
			data = (char)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsInt());
			data = currentNode_->GetInt();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<char*>(char*& data)
	{
		if (human_)
		{
			data = std::strcpy(data, hJsonNode_);
		}
		else
		{
			assert(currentNode_->IsString());
			std::strcpy(data, currentNode_->GetString());
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<byte>(byte& data)
	{
		if (human_)
		{
			data = (byte)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsUint());
			data = currentNode_->GetUint();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<short>(short& data)
	{
		if (human_)
		{
			data = (short)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsInt());
			data = currentNode_->GetInt();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<ushort>(ushort& data)
	{
		if (human_)
		{
			data = (ushort)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsUint());
			data = currentNode_->GetUint();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<int>(int& data)
	{
		if (human_)
		{
			data = (int)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsInt());
			data = currentNode_->GetInt();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<uint>(uint& data)
	{
		if (human_)
		{
			data = (uint)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsUint());
			data = currentNode_->GetUint();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<float>(float& data)
	{
		if (human_)
		{
			data = (float)hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsDouble());
			data = currentNode_->GetFloat();
		}
	}

	template<>
	inline void JsonReader::TransferPrimitive<double>(double& data)
	{
		if (human_)
		{
			data = hJsonNode_;
		}
		else
		{
			assert(currentNode_->IsDouble());
			data = currentNode_->GetDouble();
		}
	}



}