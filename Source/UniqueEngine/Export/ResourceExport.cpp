#include "UniquePCH.h"

using namespace Unique;

UNIQUE_C_API ResourceRef* ResourceRef_new(const char* type, const char* name)
{
	return new ResourceRef(type, name);
}

uExportDelete(ResourceRef)

UNIQUE_C_API ResourceRefList* ResourceRefList_new(const char* type, const char* name)
{
	return new ResourceRefList(type, name);
}

uExportDelete(ResourceRefList)

UNIQUE_C_API Resource* ResourceCache_GetResource(ResourceCache* self, const StringID& type, const char* name, bool sendEventOnFailure)
{
	return self->GetResource(type, name, sendEventOnFailure);
}
