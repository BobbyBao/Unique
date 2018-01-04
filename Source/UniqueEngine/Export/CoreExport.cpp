#include "UniquePCH.h"

using namespace Unique;

UNIQUE_C_API void* StringID_Create(const char* name)
{
	StringID id(name);
	return id.GetInternalData();
}

uExport(Context, void, RegisterSubsystem, Object*, obj)
uExport(Context, void, RemoveSubsystem, StringID, objectType)
uExport(Context, const Vector<Object*>&, GetSubsystems)
uExport(Context, Object*, CreateObject, const StringID&, type)

uExport(RefCounted, void, AddRef)
uExport(RefCounted, void, ReleaseRef)
uExport(RefCounted, int, WeakRefs)
uExport(RefCounted, int, Refs)

UNIQUE_C_API void Object_Delete(RefCount* refCount)
{
	delete refCount;
}

UNIQUE_C_API const void* Object_GetType(Object* obj)
{
	StringID id = obj->GetType();
	return id.GetInternalData();
}

// uExport(Object, void, SendEvent, const StringID&, eventType, const Event&, eventData)

UNIQUE_C_API void Object_SendEvent(Object* self, const StringID& eventType, const void* eventData)
{
	self->SendEvent(eventType, *(const Event*)eventData);
}

UNIQUE_C_API void Object_SubscribeToGlobalEvent(Object* self, const StringID& eventType, EventFn handler)
{
	self->Subscribe(eventType, new StaticEventHandler(self, handler));
}

UNIQUE_C_API void Object_SubscribeToEvent(Object* self, Object* sender, const StringID& eventType, EventFn handler)
{
	self->Subscribe(sender, eventType, new StaticEventHandler(self, handler));
}