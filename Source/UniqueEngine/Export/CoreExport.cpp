#include "UniquePCH.h"

using namespace Unique;

UNIQUE_C_API void* StringID_Create(const char* name)
{
	StringID id(name);
	return id.GetInternalData();
}

UNIQUE_C_API void Context_RegisterSubsystem(Object* obj)
{
	GetContext()->RegisterSubsystem(obj);
}

UNIQUE_C_API void Context_RemoveSubsystem(StringID objectType)
{
	GetContext()->RemoveSubsystem(objectType);
}

UNIQUE_C_API const Vector<Object*>& Context_GetSubsystems()
{
	return GetContext()->GetSubsystems();
}

UNIQUE_C_API Object* Object_Create(const StringID& type)
{
	SPtr<Object> obj = GetContext()->CreateObject(type);
	obj->AddRef();
	return obj.Get();
}

UNIQUE_C_API void Object_AddRef(RefCounted* obj)
{
	obj->AddRef();
}

UNIQUE_C_API void Object_ReleaseRef(RefCounted* obj)
{
	obj->ReleaseRef();
}

UNIQUE_C_API int Object_WeakRefs(RefCounted* obj)
{
	return obj->WeakRefs();
}

UNIQUE_C_API void Object_Delete(RefCount* refCount)
{
	delete refCount;
}

UNIQUE_C_API int Object_Refs(RefCounted* obj)
{
	return obj->Refs();
}

UNIQUE_C_API const void* Object_GetType(Object* obj)
{
	StringID id = obj->GetType();
	return id.GetInternalData();
}

UNIQUE_C_API void Object_SendEvent(Object* self, const StringID& eventType, const void* eventData)
{
	self->SendEvent(eventType, *(const Event*)eventData);
}

/// Subscribe to an event that can be sent by any sender.
UNIQUE_C_API void Object_SubscribeToGlobalEvent(Object* self, const StringID& eventType, EventFn handler)
{
	self->Subscribe(eventType, new StaticEventHandler(self, handler));
}

/// Subscribe to a specific sender's event.
UNIQUE_C_API void Object_SubscribeToEvent(Object* self, Object* sender, StringID eventType, EventFn handler)
{
	self->Subscribe(sender, eventType, new StaticEventHandler(self, handler));
}