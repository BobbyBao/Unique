#include "../Precompiled.h"
#include "Object.h"
#include "Context.h"
#include "Thread.h"
#include "../IO/Log.h"

namespace Unique
{

Context* Object::context_ = nullptr;

Object::Object()
{
}

Object::~Object()
{
    UnsubscribeFromAllEvents();
	
	if(context_)
		context_->RemoveEventSender(this);
}

template<class TransferFunction>
void Object::Transfer(TransferFunction& transfer)
{
	if (transfer.IsWriting())
	{
		StringID tmp = GetType();
		transfer.Transfer(tmp, "Type", TF_READONLY);
	}
}

void Object::OnEvent(Object* sender, StringID eventType, const Event& eventData)
{
    // Make a copy of the context pointer in case the object is destroyed during event handler invocation
    Context* context = context_;
    EventHandler* specific = 0;
    EventHandler* nonSpecific = 0;

    EventHandler* handler = eventHandlers_.first();
    while (handler)
    {
        if (handler->GetEventType() == eventType)
        {
            if (!handler->GetSender())
                nonSpecific = handler;
            else if (handler->GetSender() == sender)
            {
                specific = handler;
                break;
            }
        }
        handler = eventHandlers_.next(handler);
    }

    // Specific event handlers have priority, so if found, invoke first
    if (specific)
    {
        context->SetEventHandler(specific);
        specific->Invoke((const Event&)eventData);
        context->SetEventHandler(0);
        return;
    }

    if (nonSpecific)
    {
        context->SetEventHandler(nonSpecific);
        nonSpecific->Invoke((const Event&)eventData);
        context->SetEventHandler(0);
    }
}

bool Object::IsInstanceOf(const StringID& type) const
{
    return GetTypeInfo()->IsTypeOf(type);
}

bool Object::IsInstanceOf(const TypeInfo* info) const
{
    return GetTypeInfo()->IsTypeOf(info);
}

void Object::SubscribeToEvent(const StringID& eventType, EventHandler* handler)
{
    if (!handler)
        return;

    handler->SetSenderAndEventType(0, eventType);
    // Remove old event handler first
    EventHandler* previous;
    EventHandler* oldHandler = FindSpecificEventHandler(0, eventType, &previous);
    if (oldHandler)
    {
        eventHandlers_.erase(oldHandler, previous);
        eventHandlers_.insert_front(handler);
    }
    else
    {
        eventHandlers_.insert_front(handler);
        context_->AddEventReceiver(this, eventType);
    }
}

void Object::SubscribeToEvent(Object* sender, const StringID& eventType, EventHandler* handler)
{
    // If a null sender was specified, the event can not be subscribed to. Delete the handler in that case
    if (!sender || !handler)
    {
        delete handler;
        return;
    }

    handler->SetSenderAndEventType(sender, eventType);
    // Remove old event handler first
    EventHandler* previous;
    EventHandler* oldHandler = FindSpecificEventHandler(sender, eventType, &previous);
    if (oldHandler)
    {
        eventHandlers_.erase(oldHandler, previous);
        eventHandlers_.insert_front(handler);
    }
    else
    {
        eventHandlers_.insert_front(handler);
        context_->AddEventReceiver(this, sender, eventType);
    }
}

void Object::UnsubscribeFromEvent(StringID eventType)
{
    for (;;)
    {
        EventHandler* previous;
        EventHandler* handler = FindEventHandler(eventType, &previous);
        if (handler)
        {
            if (handler->GetSender())
                context_->RemoveEventReceiver(this, handler->GetSender(), eventType);
            else
                context_->RemoveEventReceiver(this, eventType);
            eventHandlers_.erase(handler, previous);
        }
        else
            break;
    }
}

void Object::UnsubscribeFromEvent(Object* sender, StringID eventType)
{
    if (!sender)
        return;

    EventHandler* previous;
    EventHandler* handler = FindSpecificEventHandler(sender, eventType, &previous);
    if (handler)
    {
        context_->RemoveEventReceiver(this, handler->GetSender(), eventType);
        eventHandlers_.erase(handler, previous);
    }
}

void Object::UnsubscribeFromEvents(Object* sender)
{
    if (!sender)
        return;

    for (;;)
    {
        EventHandler* previous;
        EventHandler* handler = FindSpecificEventHandler(sender, &previous);
        if (handler)
        {
            context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
            eventHandlers_.erase(handler, previous);
        }
        else
            break;
    }
}

void Object::UnsubscribeFromAllEvents()
{
	if (!context_)
	{
		return;
	}

    for (;;)
    {
        EventHandler* handler = eventHandlers_.first();
        if (handler)
        {
            if (handler->GetSender())
                context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
            else
                context_->RemoveEventReceiver(this, handler->GetEventType());
            eventHandlers_.erase(handler);
        }
        else
            break;
    }
}

void Object::UnsubscribeFromAllEventsExcept(const PODVector<StringID>& exceptions, bool onlyUserData)
{
    EventHandler* handler = eventHandlers_.first();
    EventHandler* previous = 0;

    while (handler)
    {
        EventHandler* next = eventHandlers_.next(handler);

        if ((!onlyUserData || handler->GetUserData()) && !exceptions.contains(handler->GetEventType()))
        {
            if (handler->GetSender())
                context_->RemoveEventReceiver(this, handler->GetSender(), handler->GetEventType());
            else
                context_->RemoveEventReceiver(this, handler->GetEventType());

            eventHandlers_.erase(handler, previous);
        }
        else
            previous = handler;

        handler = next;
    }
}

void Object::SendEvent(const StringID& eventType)
{
	Event noEventData;

    SendEvent(eventType, noEventData);
}

void Object::SendEvent(const StringID& eventType, const Event& eventData)
{
    if (!Thread::IsMainThread())
    {
        UNIQUE_LOGERROR("Sending events is only supported from the main thread");
        return;
    }

    // Make a weak pointer to self to check for destruction during event handling
    WPtr<Object> self(this);
    Context* context = context_;
    HashSet<Object*> processed;

    context->BeginSendEvent(this, eventType);

    // Check first the specific event receivers
    // Note: group is held alive with a shared ptr, as it may get destroyed along with the sender
    SPtr<EventReceiverGroup> group(context->GetEventReceivers(this, eventType));
    if (group)
    {
        group->BeginSendEvent();

        for (unsigned i = 0; i < group->receivers_.size(); ++i)
        {
            Object* receiver = group->receivers_[i];
            // Holes may exist if receivers removed during send
            if (!receiver)
                continue;

            receiver->OnEvent(this, eventType, eventData);

            // If self has been destroyed as a result of event handling, exit
            if (self.Expired())
            {
                group->EndSendEvent();
                context->EndSendEvent();
                return;
            }

            processed.insert(receiver);
        }

        group->EndSendEvent();
    }

    // Then the non-specific receivers
    group = context->GetEventReceivers(eventType);
    if (group)
    {
        group->BeginSendEvent();

        if (processed.empty())
        {
            for (unsigned i = 0; i < group->receivers_.size(); ++i)
            {
                Object* receiver = group->receivers_[i];
                if (!receiver)
                    continue;

                receiver->OnEvent(this, eventType, eventData);

                if (self.Expired())
                {
                    group->EndSendEvent();
                    context->EndSendEvent();
                    return;
                }
            }
        }
        else
        {
            // If there were specific receivers, check that the event is not sent doubly to them
            for (unsigned i = 0; i < group->receivers_.size(); ++i)
            {
                Object* receiver = group->receivers_[i];
                if (!receiver || processed.count(receiver))
                    continue;

                receiver->OnEvent(this, eventType, eventData);

                if (self.Expired())
                {
                    group->EndSendEvent();
                    context->EndSendEvent();
                    return;
                }
            }
        }

        group->EndSendEvent();
    }

    context->EndSendEvent();
}

bool Object::HasSubscribedToEvent(StringID eventType) const
{
    return FindEventHandler(eventType) != 0;
}

bool Object::HasSubscribedToEvent(Object* sender, StringID eventType) const
{
    if (!sender)
        return false;
    else
        return FindSpecificEventHandler(sender, eventType) != 0;
}

EventHandler* Object::FindEventHandler(StringID eventType, EventHandler** previous) const
{
    EventHandler* handler = eventHandlers_.first();
    if (previous)
        *previous = 0;

    while (handler)
    {
        if (handler->GetEventType() == eventType)
            return handler;
        if (previous)
            *previous = handler;
        handler = eventHandlers_.next(handler);
    }

    return 0;
}

EventHandler* Object::FindSpecificEventHandler(Object* sender, EventHandler** previous) const
{
    EventHandler* handler = eventHandlers_.first();
    if (previous)
        *previous = 0;

    while (handler)
    {
        if (handler->GetSender() == sender)
            return handler;
        if (previous)
            *previous = handler;
        handler = eventHandlers_.next(handler);
    }

    return 0;
}

EventHandler* Object::FindSpecificEventHandler(Object* sender, StringID eventType, EventHandler** previous) const
{
    EventHandler* handler = eventHandlers_.first();
    if (previous)
        *previous = 0;

    while (handler)
    {
        if (handler->GetSender() == sender && handler->GetEventType() == eventType)
            return handler;
        if (previous)
            *previous = handler;
        handler = eventHandlers_.next(handler);
    }

    return 0;
}

void Object::RemoveEventSender(Object* sender)
{
    EventHandler* handler = eventHandlers_.first();
    EventHandler* previous = 0;

    while (handler)
    {
        if (handler->GetSender() == sender)
        {
            EventHandler* next = eventHandlers_.next(handler);
            eventHandlers_.erase(handler, previous);
            handler = next;
        }
        else
        {
            previous = handler;
            handler = eventHandlers_.next(handler);
        }
    }
}


}
