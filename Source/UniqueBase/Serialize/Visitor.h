#pragma once
#include "IO/IStream.h"
#include "PrimitiveTypeTraits.h"

namespace Unique
{
	enum class TransferState
	{
		Reading,
		Writing,
		Resolve
	};

	class Object;

	class Visitor
	{
	public:
		Visitor(TransferState state) 
			: state_(state)
		{
		}

		bool IsReading() const { return state_ == TransferState::Reading; }

		bool IsWriting() const { return state_ == TransferState::Writing; }

		template<class T>
		bool Save(const char* fileName, T& data)
		{
			if (!StartDocument(fileName))
			{
				return false;
			}

			TypeTraits<T>::Transfer(data, *this);

			EndDocument();

			return true;
		}

		template<class T>
		bool Load(const String& fileName, T& data)
		{
			auto& cache = GetSubsystem<ResourceCache>();
			SPtr<File> file = cache.GetFile(fileName);
			if (!file)
			{
				return false;
			}

			return Load(*file, data);
		}

		template<class T>
		bool Load(IStream& source, T& data)
		{
			data_ = source.ReadAll();

			if (data_.empty())
			{
				return false;
			}

			rootObject_ = TypeTraits<T>::IsObject();
			rootType_ = TypeTraits<T>::GetTypeName();

			StartDocument(source.GetName());
			TypeTraits<T>::Transfer(data, *this);
			EndDocument();
			return true;
		}

		template<class T>
		void TransferPrimitive(T& data)
		{
			data.Transfer(*this);
		}

		template<class T>
		void TransferObject(SPtr<T>& data);

		template<class T>
		void TransferArray(T& data, int metaFlag = 0);

		template<class T>
		void TransferMap(T& data, int metaFlag = 0);

		template<class T>
		void TransferSet(T& data, int metaFlag = 0);

		template<class T>
		void TransferAttribute(const char* name, T& data, AttributeFlag metaFlag = AttributeFlag::Default)\
		{
			AttributeFlag metaFlagSave = attributeFlag_;

			attributeFlag_ = metaFlag;

			if (TypeTraits<T>::IsArray())
			{
				attributeFlag_ |= AttributeFlag::Vector;
			}

			if (TypeTraits<T>::IsMap())
			{
				attributeFlag_ |= AttributeFlag::Map;
			}

			if (StartAttribute(name))
			{
				Unique::TypeTraits<T>::Transfer(data, *this);
				EndAttribute();
			}

			attributeFlag_ = metaFlagSave;
		}

		template <typename... Rest>
		void TransferAttributes(Rest&... rest)\
		{
			int sz = sizeof ...(Rest);
			StartObject(sz / 2);
			TransferImpl1(rest...);
			EndObject();
		}

		template<class T>
		void Transfer(T& data)
		{
// 			TypeInfo* typeInfo = TypeInfo.Get<T>();
// 			if (typeInfo)
// 			{
// 				typeInfo->Transfer(*this, &data);
// 			}

			data.Transfer(*this);
		}

		virtual bool StartDocument(const String& fileName) { return true; }
		virtual void EndDocument() {}
		virtual bool StartObject(uint size) { return true; }
		virtual void EndObject() {}
		virtual SPtr<Object> CreateObject() { return nullptr; }
		virtual bool StartAttribute(const String& key) { return true; }
		virtual void EndAttribute() {}
		virtual bool StartArray(uint& size) { return true; }
		virtual void SetElement(uint index) {}
		virtual void EndArray() {}

		virtual void TransferBin(ByteArray& data) {}
		virtual void TransferPrimitive(std::string& data) {}
		virtual void TransferPrimitive(String& data) {}
		virtual void TransferPrimitive(bool& data) {}
		virtual void TransferPrimitive(char& data) {}
		virtual void TransferPrimitive(unsigned char& data) {}
		virtual void TransferPrimitive(short& data) {}
		virtual void TransferPrimitive(unsigned short& data) {}
		virtual void TransferPrimitive(int& data) {}
		virtual void TransferPrimitive(unsigned int& data) {}
		virtual void TransferPrimitive(long long& data) {}
		virtual void TransferPrimitive(unsigned long long& data) {}
		virtual void TransferPrimitive(float& data) {}
		virtual void TransferPrimitive(double& data) {}
		virtual void TransferPrimitive(Vector2& data) {}
		virtual void TransferPrimitive(Vector3& data) {}
		virtual void TransferPrimitive(Vector4& data) {}
		virtual void TransferPrimitive(Color& data) {}
		virtual void TransferPrimitive(Quaternion& data) {}


	protected:
		template <typename First, typename... Rest>
		void TransferImpl1(First first, Rest&... rest)
		{
			TransferImpl2(first, rest...);
		}

		template <typename First, typename Second, typename... Rest>
		void TransferImpl2(First name, Second& val, Rest&... rest)
		{
			TransferAttribute(name, val);
			TransferImpl1(rest...);
		}

		template <typename First, typename Second, typename... Rest>
		void TransferImpl2(First name, Second& val)
		{
			TransferAttribute(name, val);
		}

		bool rootObject_ = true;
		std::string rootType_;
		AttributeFlag attributeFlag_;
		TransferState state_;
		ByteArray data_;
	};

	template<class T>
	inline void Visitor::TransferObject(SPtr<T>& data)
	{
		if (IsReading())
		{
			if (data == nullptr)
			{
				data = StaticCast<T, Object>(CreateObject());
			}

		}

		if (data)
		{
			data->Transfer(*this);
		}

	}

	template<class T>
	inline void Visitor::TransferArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		
		uint size = 0;
		if (IsReading())
		{
			data.clear();
			if (StartArray(size))
			{
				for (uint i = 0; i < size; ++i)
				{
					SetElement(i);
					non_const_value_type val;
					TypeTraits<non_const_value_type>::Transfer(val, *this);
					data.push_back(val);
				}

				EndArray();
			}

		}
		else
		{
			size = (uint)data.size();
			if (StartArray(size))
			{
				for (non_const_value_type& val : data)
				{
					TypeTraits<non_const_value_type>::Transfer(val, *this);
				}

				EndArray();
			}

		}

	}

	template<class T>
	inline void Visitor::TransferMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		uint size = 0;
		if (IsReading())
		{
			data.clear();
			if (StartArray(size))
			{
				for (uint i = 0; i < size; ++i)
				{
					SetElement(i);
					non_const_value_type val;
					TypeTraits<non_const_value_type>::Transfer(val, *this);
					data.insert(val);
				}

				EndArray();
			}

		}
		else
		{
			size = (uint)data.size();
			if (StartArray(size))
			{
				for (non_const_value_type& val : data)
				{
					TypeTraits<non_const_value_type>::Transfer(val, *this);
				}

				EndArray();
			}

		}
	}


	template<class T>
	inline void Visitor::TransferSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		uint size = 0;
		if (IsReading())
		{
			data.clear();
			if (StartArray(size))
			{
				for (uint i = 0; i < size; ++i)
				{
					SetElement(i);
					non_const_value_type val;
					TypeTraits<non_const_value_type>::Transfer(val, *this);
					data.insert(val);
				}

				EndArray();
			}

		}
		else
		{
			size = (uint)data.size();
			if (StartArray(size))
			{
				for (non_const_value_type& val : data)
				{
					TypeTraits<non_const_value_type>::Transfer(val, *this);
				}

				EndArray();
			}

		}
	}


}
