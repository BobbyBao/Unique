#pragma once
#include "IO/IStream.h"
#include "PrimitiveTypeTraits.h"

namespace Unique
{
	enum class VisitState
	{
		Reading,
		Writing,
		Resolve
	};

	class Object;

	class Visitor
	{
	public:
		Visitor(VisitState state) 
			: state_(state)
		{
		}

		bool IsReading() const { return state_ == VisitState::Reading; }

		bool IsWriting() const { return state_ == VisitState::Writing; }

		template<class T>
		bool Save(const char* fileName, T& data)
		{
			if (!StartDocument(fileName))
			{
				return false;
			}

			TypeTraits<T>::Visit(data, *this);

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
			TypeTraits<T>::Visit(data, *this);
			EndDocument();
			return true;
		}

		template<class T>
		void VisitPrimitive(T& data)
		{
			data.Visit(*this);
		}

		template<class T>
		void VisitObject(SPtr<T>& data);

		template<class T>
		void VisitArray(T& data, int metaFlag = 0);

		template<class T>
		void VisitMap(T& data, int metaFlag = 0);

		template<class T>
		void VisitSet(T& data, int metaFlag = 0);

		template<class T>
		void VisitAttribute(const char* name, T& data, AttributeFlag metaFlag = AttributeFlag::Default)\
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
				Unique::TypeTraits<T>::Visit(data, *this);
				EndAttribute();
			}

			attributeFlag_ = metaFlagSave;
		}

		template <typename... Rest>
		void VisitAttributes(Rest&... rest)\
		{
			int sz = sizeof ...(Rest);
			StartObject(sz / 2);
			VisitImpl1(rest...);
			EndObject();
		}

		template<class T>
		void Visit(T& data)
		{
// 			TypeInfo* typeInfo = TypeInfo.Get<T>();
// 			if (typeInfo)
// 			{
// 				typeInfo->Visit(*this, &data);
// 			}

			data.Visit(*this);
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

		virtual void VisitBin(ByteArray& data) {}
		virtual void VisitPrimitive(std::string& data) {}
		virtual void VisitPrimitive(String& data) {}
		virtual void VisitPrimitive(bool& data) {}
		virtual void VisitPrimitive(char& data) {}
		virtual void VisitPrimitive(unsigned char& data) {}
		virtual void VisitPrimitive(short& data) {}
		virtual void VisitPrimitive(unsigned short& data) {}
		virtual void VisitPrimitive(int& data) {}
		virtual void VisitPrimitive(unsigned int& data) {}
		virtual void VisitPrimitive(long long& data) {}
		virtual void VisitPrimitive(unsigned long long& data) {}
		virtual void VisitPrimitive(float& data) {}
		virtual void VisitPrimitive(double& data) {}
		virtual void VisitPrimitive(Vector2& data) {}
		virtual void VisitPrimitive(Vector3& data) {}
		virtual void VisitPrimitive(Vector4& data) {}
		virtual void VisitPrimitive(Color& data) {}
		virtual void VisitPrimitive(Quaternion& data) {}


	protected:
		template <typename First, typename... Rest>
		void VisitImpl1(First first, Rest&... rest)
		{
			VisitImpl2(first, rest...);
		}

		template <typename First, typename Second, typename... Rest>
		void VisitImpl2(First name, Second& val, Rest&... rest)
		{
			VisitAttribute(name, val);
			VisitImpl1(rest...);
		}

		template <typename First, typename Second, typename... Rest>
		void VisitImpl2(First name, Second& val)
		{
			VisitAttribute(name, val);
		}

		bool rootObject_ = true;
		std::string rootType_;
		AttributeFlag attributeFlag_;
		VisitState state_;
		ByteArray data_;
	};

	template<class T>
	inline void Visitor::VisitObject(SPtr<T>& data)
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
			data->Visit(*this);
		}

	}

	template<class T>
	inline void Visitor::VisitArray(T& data, int metaFlag)
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
					TypeTraits<non_const_value_type>::Visit(val, *this);
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
					TypeTraits<non_const_value_type>::Visit(val, *this);
				}

				EndArray();
			}

		}

	}

	template<class T>
	inline void Visitor::VisitMap(T& data, int metaFlag)
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
					TypeTraits<non_const_value_type>::Visit(val, *this);
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
					TypeTraits<non_const_value_type>::Visit(val, *this);
				}

				EndArray();
			}

		}
	}


	template<class T>
	inline void Visitor::VisitSet(T& data, int metaFlag)
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
					TypeTraits<non_const_value_type>::Visit(val, *this);
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
					TypeTraits<non_const_value_type>::Visit(val, *this);
				}

				EndArray();
			}

		}
	}


}
