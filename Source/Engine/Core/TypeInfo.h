#pragma once
#include "StringID.h"

namespace Unique
{
	class Object;
	typedef SPtr<Object>(*CreateObjectFn)();

	/// Type info.
	class UNIQUE_API TypeInfo
	{
	public:
		/// Construct.
		TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo, CreateObjectFn = nullptr);
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

		SPtr<Object> CreateObject() const;

		static int GetTypeInfo(TypeInfo** typeInfoList);
		static void Init();
	private:
		/// Type.
		StringID type_;
		// Category
		StringID category_;
		/// Base class type info.
		const TypeInfo* baseTypeInfo_;
		// Create object
		CreateObjectFn createObjectFn_;
	};

}