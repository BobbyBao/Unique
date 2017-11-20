#pragma once
#include "../Core/CoreDefs.h"
#include "../Container/refcounted.h"
#include "../Serialize/BinaryReader.h"
#include "../Serialize/BinaryWriter.h"
#include "../Serialize/JsonReader.h"
#include "../Serialize/JsonWriter.h"

namespace Unique
{

	class Attribute
	{
	public:
		Attribute(const String& name, uint mode)
			: name_(name), mode_(mode)
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
		uint mode_;
		uint type_;
	};

	template<class T>
	class TAttribute : public Attribute
	{
	public:
		TAttribute(const String& name, uint offset, unsigned mode)
			: Attribute(name, mode), offset_(offset)
		{
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
			visitor.TransferAttribute(*dest, name_.CString(), mode_);
		}

		uint offset_;
	};


	/// Attribute trait (default use const reference for object type).
	template <typename T> struct AttributeTrait
	{
		/// Get function return type.
		typedef const T& ReturnType;
		/// Set function parameter type.
		typedef const T& ParameterType;
	};

	/// Int attribute trait.
	template <> struct AttributeTrait<int>
	{
		typedef int ReturnType;
		typedef int ParameterType;
	};

	/// unsigned attribute trait.
	template <> struct AttributeTrait<unsigned>
	{
		typedef unsigned ReturnType;
		typedef unsigned ParameterType;
	};

	/// Bool attribute trait.
	template <> struct AttributeTrait<bool>
	{
		typedef bool ReturnType;
		typedef bool ParameterType;
	};

	/// Float attribute trait.
	template <> struct AttributeTrait<float>
	{
		typedef float ReturnType;
		typedef float ParameterType;
	};

	/// Template implementation of the attribute accessor invoke helper class.
	template <typename T, typename U, typename Trait> class AttributeAccessorImpl : public Attribute
	{
	public:
		typedef typename Trait::ReturnType(T::*GetFunctionPtr)() const;
		typedef void (T::*SetFunctionPtr)(typename Trait::ParameterType);

		/// Construct with function pointers.
		AttributeAccessorImpl(const String& name, GetFunctionPtr getFunction, SetFunctionPtr setFunction, unsigned mode) :
			getFunction_(getFunction),
			setFunction_(setFunction),
			Attribute(name, mode)
		{
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
				visitor.TransferAttribute(value, name_.CString(), mode_);
				(classPtr->*setFunction_)(value);
			}

			if (visitor.IsWriting())
			{
				U value = (classPtr->*getFunction_)();
				visitor.TransferAttribute(value, name_.CString(), mode_);
			}
		}

		GetFunctionPtr getFunction_;
		SetFunctionPtr setFunction_;
	};


	/// Define an attribute that points to a memory offset in the object.
#define uAttribute(name, variable, typeName, mode)\
	ClassName::GetTypeInfoStatic()->RegisterAttribute(new Unique::TAttribute<typeName>(name, offsetof(ClassName, variable), mode));

/// Define an attribute that uses get and set functions.
#define uAccessor(name, getFunction, setFunction, typeName, mode)\
	ClassName::GetTypeInfoStatic()->RegisterAttribute(new Unique::AttributeAccessorImpl<ClassName, typeName, Unique::AttributeTrait<typeName > >(name, &ClassName::getFunction, &ClassName::setFunction, mode));

}
