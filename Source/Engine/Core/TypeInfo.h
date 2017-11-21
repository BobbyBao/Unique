#pragma once
#include "../Container/StringID.h"

namespace Unique
{
	class Attribute;
	
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

		void RegisterAttribute(Attribute* attr);

		const std::vector<UPtr<Attribute>>& GetAttributes() const { return attributes_;}
		
	private:
		/// Type.
		StringID type_;
		// Category
		StringID category_;
		/// Base class type info.
		const TypeInfo* baseTypeInfo_;

		std::vector<UPtr<Attribute>> attributes_;
	};

}