#include "Precompiled.h"
#include "resourceref.h"

namespace Unique
{

/// Construct.
ResourceRef::ResourceRef()
{
}

/// Construct with type only and empty id.
ResourceRef::ResourceRef(StringID type) :
	type_(type)
{
}

/// Construct with type and resource name.
ResourceRef::ResourceRef(StringID type, const String& name) :
	type_(type),
	name_(name)
{
}

/// Construct with type and resource name.
ResourceRef::ResourceRef(const String& type, const String& name) :
	type_(type),
	name_(name)
{
}

/// Construct with type and resource name.
ResourceRef::ResourceRef(const char* type, const char* name) :
	type_(type),
	name_(name)
{
}

/// Construct from another ResourceRef.
ResourceRef::ResourceRef(const ResourceRef& rhs) :
	type_(rhs.type_),
	name_(rhs.name_)
{
}


}
