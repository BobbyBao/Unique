#pragma once
#include "../Container/StringID.h"
#include "../Container/Container.h"
#include "Attribute.h"

namespace Unique
{
	/// Type info.
	class UNIQUE_API TypeInfo
	{
	public:
		/// Construct.
		TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo);
		/// Destruct.
		~TypeInfo();

		/// Check current type is type of specified type.
		bool IsTypeOf(const StringID& type) const;
		/// Check current type is type of specified type.
		bool IsTypeOf(const TypeInfo* typeInfo) const;
		/// Check current type is type of specified class type.
		template<typename T> bool IsTypeOf() const { return IsTypeOf(T::GetTypeInfoStatic()); }

		/// Return type.
		const StringID& GetType() const { return type_; }
		/// Return base type info.
		const TypeInfo* GetBaseTypeInfo() const { return baseTypeInfo_; }
		
		/*
		template<class C, class T>
		void RegisterAttribute(const char* name, T C::* m, AttributeFlag flag = AttributeFlag::Default)
		{
			RegisterAttribute(
				new Unique::TAttribute<T>(name, OffsetOf(m), flag));
		}*/

		template<class T>
		void RegisterAttribute(const char* name, size_t offset, AttributeFlag flag = AttributeFlag::Default)
		{
			RegisterAttribute(
				new Unique::TAttribute<T>(name, offset, flag));
		}

		template<typename T, typename SET>
		void RegisterAccessor(const char* name, size_t offset, SET setter, AttributeFlag flag = AttributeFlag::Default)
		{
			RegisterAttribute(
				new Unique::AttributeImpl<function_traits<SET>::ClassType, T>(name, offset, setter, flag));
		}

		template<typename GET, typename SET>
		void RegisterAccessor(const char* name, GET getter, SET setter, AttributeFlag flag = AttributeFlag::Default)
		{
			RegisterAttribute(
				new Unique::AttributeAccessorImpl<function_traits<GET>::ClassType, function_traits<GET>::RawType, function_traits<GET>>(name, getter, setter, flag));
		}

		template<typename GET, typename SET>
		void RegisterMixedAccessor(const char* name, GET getter, SET setter, AttributeFlag flag = AttributeFlag::Default)
		{
			RegisterAttribute(
				new Unique::AttributeAccessorImpl<mixed_function_traits<GET>::ClassType, mixed_function_traits<GET>::RawType, mixed_function_traits<GET>>(name, getter, setter, flag));
		}

		void RegisterAttribute(Attribute* attr);

		const Vector<Attribute*>& GetAttributes() const { return attributes_;}

		template<class T>
		static const Unique::StringID& GetTypeID()
		{
			static const Unique::StringID s_typeID(GetTypeName<T>());
			return s_typeID;
		}

		template<class T>
		static const char* GetTypeName()
		{
			static const char* s_typeName = GetTypeName(typeid(T).name());
			return s_typeName;
		}

		static void RegisterTypeInfo();
		static int GetAllTypeInfo(TypeInfo** typeInfos);
		static TypeInfo* GetTypeInfo(const StringID& type);
	private:

		static const char* GetTypeName(const char* name)
		{
			const char* p = std::strrchr(name, ':');
			return p ? ++p : name;
		}

		/// Type.
		StringID type_;
		// Category
		StringID category_;
		/// Base class type info.
		const TypeInfo* baseTypeInfo_;

		Vector<Attribute*> attributes_;
	};


	template<typename T>
	struct Type
	{
		static TypeInfo* Info() { static TypeInfo s_typeInfo(TypeInfo::GetTypeName<T>(), nullptr); return &s_typeInfo; }
	};


#define uRTTI(typeName, baseTypeName) \
    public: \
        typedef typeName ClassName; \
        typedef baseTypeName BaseClassName; \
        virtual const Unique::StringID& GetType() const { return GetTypeInfoStatic()->GetType(); } \
		virtual const Unique::TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); } \
        static const Unique::StringID& GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
		static Unique::TypeInfo* GetTypeInfoStatic() { static Unique::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \
		static void RegisterObject(Context* context);

#define uObject(typeName)\
		static RegisterRuntime s_##typeName##Callbacks(&typeName::RegisterObject, nullptr);\
		void typeName::RegisterObject(Context* context)

#define uFactory(...)\
		GetContext()->RegisterFactory<ClassName>(##__VA_ARGS__);


	/// Define an attribute that points to a memory offset in the object.
//#define uAttribute(name, variable, ...)\
//	ClassName::GetTypeInfoStatic()->RegisterAttribute(name, &ClassName::variable, ##__VA_ARGS__);

#define uAttribute(name, variable, ...)\
	ClassName::GetTypeInfoStatic()->RegisterAttribute<decltype(((ClassName*)0)->variable)>(name, offsetof(ClassName, variable), ##__VA_ARGS__);

#define uAccessor(name, getFunction, setFunction, ...)\
	ClassName::GetTypeInfoStatic()->RegisterAccessor(name, &ClassName::getFunction, &ClassName::setFunction, ##__VA_ARGS__);

#define uMixedAccessor(name, getFunction, setFunction, ...)\
	ClassName::GetTypeInfoStatic()->RegisterMixedAccessor(name, &ClassName::getFunction, &ClassName::setFunction, ##__VA_ARGS__);
	
//	static Unique::TypeInfo typeName##TypeInfoStatic(#typeName, nullptr); \

#define uStruct(typeName)\
	template<class ClassName>\
void typeName##RegisterTypeInfo(TypeInfo* typeInfo);\
		static void typeName##RegisterObject(Context* context){ typeName##RegisterTypeInfo<typeName>(Type<typeName>::Info()); }\
		static RegisterRuntime s_##typeName##Callbacks(typeName##RegisterObject, nullptr);\
		template<class ClassName>\
		void typeName##RegisterTypeInfo(TypeInfo* typeInfo)

#define uProperty(name, type, variable, ...)\
	typeInfo->RegisterAttribute<type>(name, offsetof(ClassName, variable), ##__VA_ARGS__);

}