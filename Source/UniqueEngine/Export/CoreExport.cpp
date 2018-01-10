#include "UniquePCH.h"

using namespace Unique;

UNIQUE_C_API void* StringID_new(const char* name)
{
	return StringID(name).GetInternalData();
}

uExport(Context, void, RegisterSubsystem, Object*, obj)
uExport(Context, void, RemoveSubsystem, StringID, objectType)
//uExport(Context, const Vector<Object*>&, GetSubsystems)

typedef void(__stdcall *SubsystemFn)(Object* subsystem);

UNIQUE_C_API void Context_GetSubsystems(Context* context, SubsystemFn fn)
{
	auto& subsystems = context->GetSubsystems();
	for (size_t i = 0; i < subsystems.size(); i++)
	{
		fn(subsystems[i]);
	}
}

UNIQUE_C_API Object* Context_CreateObject(Context* context, const StringID& type)
{
	SPtr<Object> ret = context->CreateObject(type);
	ret->AddRef();
	return ret.Get();
}

uExport(RefCounted, void, AddRef)
uExport(RefCounted, void, ReleaseRef)
uExport(RefCounted, int, WeakRefs)
uExport(RefCounted, int, Refs)

uExportDelete(RefCount)

UNIQUE_C_API const void* Object_GetType(Object* obj)
{
	StringID id = obj->GetType();
	return id.GetInternalData();
}

//uExport(Object, void, SendEvent, const StringID&, eventType, const Event&, eventData)

UNIQUE_C_API void Object_SendEvent(Object* self, const StringID& eventType, const void* eventData)
{
	self->SendEvent(eventType, *(const Event*)eventData);
}

UNIQUE_C_API void Object_Subscribe(Object* self, const StringID& eventType, EventFn handler)
{
	self->Subscribe(eventType, new StaticEventHandler(self, handler));
}

UNIQUE_C_API void Object_SubscribeTo(Object* self, Object* sender, const StringID& eventType, EventFn handler)
{
	self->SubscribeTo(sender, eventType, new StaticEventHandler(self, handler));
}