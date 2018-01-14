#pragma once
#include "../container/stringid.h"
#include "../resource/guid.h"

namespace Unique
{
	/// Vector of strings.
	typedef Vector<String> StringVector;

	/// Typed resource reference.
	struct UNIQUE_API ResourceRef
	{
		/// Construct.
		ResourceRef();
		/// Construct with type only and empty id.
		ResourceRef(const StringID& type);
		/// Construct with type and resource name.
		ResourceRef(const StringID& type, const String& name);
		/// Construct with type and resource name.
		ResourceRef(const String& type, const String& name);
		/// Construct with type and resource name.
		ResourceRef(const char* type, const char* name);

		/// Construct from another ResourceRef.
		ResourceRef(const ResourceRef& rhs);

		template<class T>
		static ResourceRef Create(const String& name)
		{
			return ResourceRef(T::GetTypeStatic(), name);
		}

		/// Object type.
		StringID type_;
		/// Object name.
		String name_;
		
		/// Test for equality with another reference.
		bool operator ==(const ResourceRef& rhs) const { return type_ == rhs.type_ && name_ == rhs.name_; }

		/// Test for inequality with another reference.
		bool operator !=(const ResourceRef& rhs) const { return type_ == rhs.type_ && name_ != rhs.name_; }

		uClass("Type", type_, "Name", name_);
		
	};

	/// %List of typed resource references.
	struct UNIQUE_API ResourceRefList
	{
		/// Construct.
		ResourceRefList()
		{
		}

		/// Construct with type only.
		ResourceRefList(const StringID& type);
		/// Construct with type and id list.
		ResourceRefList(const StringID& type, const StringVector& names);
		/// Construct with type and id list.
		ResourceRefList(const char* type, const char* names);

		/// Object type.
		StringID type_;
		/// List of object names.
		StringVector names_;

		/// Test for equality with another reference list.
		bool operator ==(const ResourceRefList& rhs) const { return type_ == rhs.type_ && names_ == rhs.names_; }

		/// Test for inequality with another reference list.
		bool operator !=(const ResourceRefList& rhs) const { return type_ != rhs.type_ || names_ != rhs.names_; }

		template<class T>
		static ResourceRefList Create(const StringVector& names)
		{
			return ResourceRefList(T::GetTypeStatic(), names);
		}

		uClass("Type", type_, "Names", names_);

	};
}

