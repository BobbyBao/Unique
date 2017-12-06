#pragma once
#include "../Core/CoreDefs.h"
#include "../Container/refcounted.h"
#include "../Serialize/BinaryReader.h"
#include "../Serialize/BinaryWriter.h"
#include "../Serialize/JsonReader.h"
#include "../Serialize/JsonWriter.h"
#include "AttributeTraits.h"

namespace Unique
{

	class Attribute : public RefCounted
	{
	public:
		Attribute(const String& name, AttributeFlag flag)
			: name_(name), flag_(flag)
		{
		}

		virtual void Visit(BinaryWriter& serializer, void* obj) {}
		virtual void Visit(BinaryReader& serializer, void* obj) {}
		virtual void Visit(JsonWriter& serializer, void* obj) {}
		virtual void Visit(JsonReader& serializer, void* obj) {}

		virtual void Get(const void* ptr, void* dest) const = 0;
		virtual void Set(void* ptr, const void* value) = 0;

		/// Name.
		String name_;
		AttributeFlag flag_;
		uint type_;
	};

	template<class T>
	class TAttribute : public Attribute
	{
	public:
		TAttribute(const String& name, size_t offset, AttributeFlag flag)
			: Attribute(name, flag), offset_(offset)
		{
			if (SerializeTraits<T>::IsArray())
			{
				flag_ |= AttributeFlag::Vector;
			}

			if (SerializeTraits<T>::IsMap())
			{
				flag_ |= AttributeFlag::Map;
			}
		}

		virtual void Visit(BinaryWriter& serializer, void* obj)
		{
			VisitImpl(serializer, obj);
		}

		virtual void Visit(BinaryReader& deserializer, void* obj)
		{
			VisitImpl(deserializer, obj);
		}

		virtual void Visit(JsonWriter& serializer, void* obj)
		{
			VisitImpl(serializer, obj);
		}

		virtual void Visit(JsonReader& deserializer, void* obj)
		{
			VisitImpl(deserializer, obj);
		}

		virtual void Get(const void* ptr, void* dest) const
		{
			const void* src = reinterpret_cast<const unsigned char*>(ptr) + offset_;
			*((T*)dest) = *((T*)src);
		}

		virtual void Set(void* ptr, const void* value)
		{
			void* dest = reinterpret_cast<unsigned char*>(ptr) + offset_;
			*((T*)dest) = *((T*)value);
		}

	protected:
		template<class Visitor>
		void VisitImpl(Visitor& visitor, void* ptr)
		{
			T* dest = (T*)(reinterpret_cast<unsigned char*>(ptr) + offset_);
			visitor.TransferAttribute(name_.CString(), *dest, flag_);
		}

		size_t offset_;
	};
	
	/// Template implementation of the attribute accessor invoke helper class.
	template <typename T, typename U, typename Trait> class AttributeAccessorImpl : public Attribute
	{
	public:
		typedef typename Trait::ReturnType(T::*GetFunctionPtr)() const;
		typedef void (T::*SetFunctionPtr)(typename Trait::ParameterType);

		/// Construct with function pointers.
		AttributeAccessorImpl(const String& name, GetFunctionPtr getFunction, SetFunctionPtr setFunction, AttributeFlag flag) :
			getFunction_(getFunction),
			setFunction_(setFunction),
			Attribute(name, flag)
		{
			if (SerializeTraits<T>::IsArray())
			{
				flag_ |= AttributeFlag::Vector;
			}

			if (SerializeTraits<T>::IsMap())
			{
				flag_ |= AttributeFlag::Map;
			}
			assert(getFunction_);
			assert(setFunction_);
		}

		virtual void Visit(BinaryWriter& serializer, void* obj)
		{
			VisitImpl(serializer, obj);
		}

		virtual void Visit(BinaryReader& deserializer, void* obj)
		{
			VisitImpl(deserializer, obj);
		}

		virtual void Visit(JsonWriter& serializer, void* obj)
		{
			VisitImpl(serializer, obj);
		}

		virtual void Visit(JsonReader& deserializer, void* obj)
		{
			VisitImpl(deserializer, obj);
		}

		/// Invoke getter function.
		virtual void Get(const void* ptr, void* dest) const
		{
			assert(ptr);
			const T* classPtr = static_cast<const T*>(ptr);
			*(U*)dest = (classPtr->*getFunction_)();
		}

		/// Invoke setter function.
		virtual void Set(void* ptr, const void* value)
		{
			assert(ptr);
			T* classPtr = static_cast<T*>(ptr);
			(classPtr->*setFunction_)(*(U*)value);
		}
	protected:
		template<class Visitor>
		void VisitImpl(Visitor& visitor, void* ptr)
		{
			assert(ptr);
			T* classPtr = static_cast<T*>(ptr);

			if (visitor.IsReading())
			{
				U value;
				visitor.TransferAttribute(name_.CString(), value, flag_);
				(classPtr->*setFunction_)(value);
			}

			if (visitor.IsWriting())
			{
				U value = (classPtr->*getFunction_)();
				visitor.TransferAttribute(name_.CString(), value, flag_);
			}
		}

		GetFunctionPtr getFunction_;
		SetFunctionPtr setFunction_;
	};


}
