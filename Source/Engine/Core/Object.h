#pragma once
#include "../Container/Ptr.h"
#include "../Container/Vector.h"
#include "../Container/LinkedList.h"
#include "../Container/StringID.h"
#include "../serialize/SerializeDefs.h"
#include "../Core/TypeInfo.h"
#include "../Core/Event.h"

#include <functional>

namespace Unique
{

class Context;
class EventHandler;

typedef SPtr<Object> (*CreateObjectFn)(); 

#define UNIQUE_OBJECT(typeName, baseTypeName) \
    public: \
        typedef typeName ClassName; \
        typedef baseTypeName BaseClassName; \
        virtual const Unique::StringID& GetType() const { return GetTypeInfoStatic()->GetType(); } \
		virtual const Unique::TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); } \
        static const Unique::StringID& GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
		static const Unique::TypeInfo* GetTypeInfoStatic() { static const Unique::TypeInfo typeInfoStatic(#typeName, BaseClassName::GetTypeInfoStatic()); return &typeInfoStatic; } \

#define DECLARE_OBJECT(typeName)\
    public: \
        typedef typeName ClassName; \
        virtual Unique::StringHash GetType() const { return GetTypeInfoStatic()->GetType(); } \
        virtual const Unique::String& GetTypeName() const { return GetTypeInfoStatic()->GetTypeName(); } \
        virtual const Unique::TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); } \
        static Unique::StringHash GetTypeStatic() { return GetTypeInfoStatic()->GetType(); } \
        static const Unique::String& GetTypeNameStatic() { return GetTypeInfoStatic()->GetTypeName(); } \
		static const Unique::TypeInfo* GetTypeInfoStatic();\
		static void RegisterObject(Context* context);

#define IMPLEMENT_OBJECT(typeName, baseTypeName)\
        const Unique::TypeInfo* typeName::GetTypeInfoStatic() { static const Unique::TypeInfo typeInfoStatic(#typeName, baseTypeName::GetTypeInfoStatic()); return &typeInfoStatic; } \
		static RegisterRuntime s_##typeName##Callbacks(typeName::RegisterObject, nullptr);\
		void typeName::RegisterObject(Context* context)
	
/// Base class for objects with type identification, subsystem access and event sending/receiving capability.
class UNIQUE_API Object : public RefCounted
{
public:
    /// Construct.
    Object();
    /// Destruct. Clean up self from event sender & receiver structures.
    virtual ~Object();

    /// Return type hash.
    virtual const StringID& GetType() const { return GetTypeInfoStatic()->GetType(); }
    /// Return type info.
    virtual const TypeInfo* GetTypeInfo() const { return GetTypeInfoStatic(); }
    /// Handle event.
    virtual void OnEvent(Object* sender, StringID eventType, const Event& eventData);
	
	template<class TransferFunction>
	void Transfer(TransferFunction& transfer);

    /// Return type info static.
    static const TypeInfo* GetTypeInfoStatic() { static const Unique::TypeInfo typeInfoStatic("Object", nullptr); return &typeInfoStatic; }
    /// Check current instance is type of specified type.
    bool IsInstanceOf(const StringID& type) const;
    /// Check current instance is type of specified type.
    bool IsInstanceOf(const TypeInfo* typeInfo) const;
    /// Check current instance is type of specified class.
    template<typename T> bool IsInstanceOf() const { return IsInstanceOf(T::GetTypeInfoStatic()); }
	
	/// Send event to all subscribers.
	void SendEvent(const StringID& eventType);
	/// Send event with parameters to all subscribers.
	void SendEvent(const StringID& eventType, const Event& eventData);

    /// Return execution context.
    static Context* GetContext() { return context_; }

    /// Return whether has subscribed to an event without specific sender.
    bool HasSubscribedToEvent(StringID eventType) const;
    /// Return whether has subscribed to a specific sender's event.
    bool HasSubscribedToEvent(Object* sender, StringID eventType) const;
    /// Return whether has subscribed to any event.
    bool HasEventHandlers() const { return !eventHandlers_.empty(); }

	template<class T, class E>
	void SubscribeToEvent(const StringID& eventType, void(T::*f)(StringID, const E&))
	{
		SubscribeToEvent(eventType, new TEventHandler<T, E>((T*)this, f));
	}

	template<class T, class E>
	void SubscribeToEvent(Object* sender, const StringID& eventType, void(T::*f)(StringID, const E&))
	{
		SubscribeToEvent(sender, eventType, new TEventHandler<T, E>((T*)this, f));
	}

	/// Subscribe to an event that can be sent by any sender.
	void SubscribeToEvent(const StringID& eventType, EventHandler* handler);
	/// Subscribe to a specific sender's event.
	void SubscribeToEvent(Object* sender, const StringID& eventType, EventHandler* handler);

protected:
	/// Unsubscribe from an event.
	void UnsubscribeFromEvent(StringID eventType);
	/// Unsubscribe from a specific sender's event.
	void UnsubscribeFromEvent(Object* sender, StringID eventType);
	/// Unsubscribe from a specific sender's events.
	void UnsubscribeFromEvents(Object* sender);
	/// Unsubscribe from all events.
	void UnsubscribeFromAllEvents();
	/// Unsubscribe from all events except those listed, and optionally only those with userdata (script registered events.)
	void UnsubscribeFromAllEventsExcept(const PODVector<StringID>& exceptions, bool onlyUserData);

    /// Execution context.
    static Context* context_;

private:
    /// Find the first event handler with no specific sender.
    EventHandler* FindEventHandler(StringID eventType, EventHandler** previous = 0) const;
    /// Find the first event handler with specific sender.
    EventHandler* FindSpecificEventHandler(Object* sender, EventHandler** previous = 0) const;
    /// Find the first event handler with specific sender and event type.
    EventHandler* FindSpecificEventHandler(Object* sender, StringID eventType, EventHandler** previous = 0) const;
    /// Remove event handlers related to a specific sender.
    void RemoveEventSender(Object* sender);

    /// Event handlers. Sender is null for non-specific handlers.
    LinkedList<EventHandler> eventHandlers_;

	friend class Context;

};

template <class T> T& Subsystem() { return *Object::GetContext()->Subsystem<T>(); }
template <class T> bool HasSubsystem() { return Object::GetContext()->Subsystem<T>() != nullptr; }

UNIQUE_API SPtr<Object> CreateObject(const StringID& type);
template <class T> SPtr<T> CreateObject() { return DynamicCast<T>(CreateObject(T::GetTypeStatic())); }


/// Base class for object factories.
class UNIQUE_API ObjectFactory
{
public:
	/// Create an object. Implemented in templated subclasses.
	virtual SPtr<Object> CreateObject() = 0;
	
	/// Return type info of objects created by this factory.
	const TypeInfo* GetTypeInfo() const { return typeInfo_; }

	/// Return type hash of objects created by this factory.
	StringID GetType() const { return typeInfo_->GetType(); }
	
protected:
	/// Type info.
	const TypeInfo* typeInfo_;
};

/// Template implementation of the object factory.
template <class T> class ObjectFactoryImpl : public ObjectFactory
{
public:
	/// Construct.
	ObjectFactoryImpl()
	{
		typeInfo_ = T::GetTypeInfoStatic();
	}

	/// Create an object of the specific type.
	virtual SPtr<Object> CreateObject() { return SPtr<Object>(new T()); }
};

}
