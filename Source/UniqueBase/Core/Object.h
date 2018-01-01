#pragma once
#include "../Container/Ptr.h"
#include "../Container/LinkedList.h"
#include "../Container/StringID.h"
#include "../serialize/SerializeTraits.h"
#include "../Core/TypeInfo.h"
#include "../Core/Event.h"


namespace Unique
{

class Context;
class EventHandler;
	
/// Base class for objects with type identification, serialize and event sending/receiving capability.
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
    static TypeInfo* GetTypeInfoStatic() { return Type<Object>::Info(); }
    /// Check current instance is type of specified type.
    bool IsInstanceOf(const StringID& type) const;
    /// Check current instance is type of specified type.
    bool IsInstanceOf(const TypeInfo* typeInfo) const;
    /// Check current instance is type of specified class.
    template<typename T> bool IsInstanceOf() const { return IsInstanceOf(T::GetTypeInfoStatic()); }

	/// Send event with parameters to all subscribers.
	template<class E>
	void SendEvent(const E& eventData)
	{
		SendEvent(E::Type(), eventData);
	}

	/// Send event to all subscribers.
	void SendEvent(const StringID& eventType);
	/// Send event with parameters to all subscribers.
	void SendEvent(const StringID& eventType, const Event& eventData);
    /// Return whether has subscribed to an event without specific sender.
    bool HasSubscribed(StringID eventType) const;
    /// Return whether has subscribed to a specific sender's event.
    bool HasSubscribed(Object* sender, StringID eventType) const;
    /// Return whether has subscribed to any event.
    bool HasEventHandlers() const { return !eventHandlers_.empty(); }

	template<class T, class E>
	void Subscribe(void(T::*f)(const E&))
	{
		Subscribe(E::Type(), new TEventHandler<T, E>((T*)this, f));
	}

	template<class T, class E>
	void Subscribe(const StringID& eventType, void(T::*f)(const E&))
	{
		Subscribe(eventType, new TEventHandler<T, E>((T*)this, f));
	}

	template<class T, class E>
	void Subscribe(Object* sender, void(T::*f)(const E&))
	{
		Subscribe(sender, E::Type(), new TEventHandler<T, E>((T*)this, f));
	}

	template<class T, class E>
	void Subscribe(Object* sender, const StringID& eventType, void(T::*f)(const E&))
	{
		Subscribe(sender, eventType, new TEventHandler<T, E>((T*)this, f));
	}

	/// Subscribe to an event that can be sent by any sender.
	void Subscribe(const StringID& eventType, EventHandler* handler);
	/// Subscribe to a specific sender's event.
	void Subscribe(Object* sender, const StringID& eventType, EventHandler* handler);

protected:

	template<class E>
	void Unsubscribe()
	{
		Unsubscribe(E::Type());
	}

	template<class E>
	void Unsubscribe(Object* sender)
	{
		Unsubscribe(sender, E::Type());
	}

	/// Unsubscribe from an event.
	void Unsubscribe(StringID eventType);
	/// Unsubscribe from a specific sender's event.
	void Unsubscribe(Object* sender, StringID eventType);
	/// Unsubscribe from a specific sender's events.
	void UnsubscribeFromEvents(Object* sender);
	/// Unsubscribe from all events.
	void UnsubscribeFromAllEvents();
	/// Unsubscribe from all events except those listed, and optionally only those with userdata (script registered events.)
	void UnsubscribeFromAllEventsExcept(const PODVector<StringID>& exceptions, bool onlyUserData);

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

/// Base class for object factories.
class UNIQUE_API ObjectFactory
{
public:
	/// Create an object. Implemented in templated subclasses.
	virtual SPtr<Object> CreateObject() = 0;

	/// Return type hash of objects created by this factory.
	const StringID& GetType() const { return typeInfo_->GetType(); }
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
