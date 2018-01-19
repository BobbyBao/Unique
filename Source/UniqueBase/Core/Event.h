#ifndef _SRC_EVENT_EVENT_HANDLER_HPP_
#define _SRC_EVENT_EVENT_HANDLER_HPP_

#include "../Container/linkedlist.h"
#include "../Container/StringID.h"
#include "TypeInfo.h"

namespace Unique
{
	class Object;

	struct Event
	{
	};

	template<typename T>
	struct TEvent : Event
	{
		static const Unique::StringID& Type()
		{
			return TypeInfo::GetTypeID<T>();
		}
	};

	/// Internal helper class for invoking event handler functions.
	class UNIQUE_API EventHandler : public LinkedListNode
	{
	public:
		/// Construct with specified receiver and userdata.
		EventHandler(Object* receiver, void* userData = 0) :
			receiver_(receiver),
			sender_(0),
			userData_(userData)
		{
		}

		/// Destruct.
		virtual ~EventHandler() { }

		/// Set sender and event type.
		void SetSenderAndEventType(Object* sender, StringID eventType)
		{
			sender_ = sender;
			eventType_ = eventType;
		}

		/// Invoke event handler function.
		virtual void Invoke(const Event& eventData) = 0;

		/// Return event receiver.
		Object* GetReceiver() const { return receiver_; }

		/// Return event sender. Null if the handler is non-specific.
		Object* GetSender() const { return sender_; }

		/// Return event type.
		const StringID& GetEventType() const { return eventType_; }

		/// Return userdata.
		void* GetUserData() const { return userData_; }

	protected:
		/// Event receiver.
		Object* receiver_;
		/// Event sender.
		Object* sender_;
		/// Event type.
		StringID eventType_;
		/// Userdata.
		void* userData_;
	};

	/// Template implementation of the event handler invoke helper (stores a function pointer of specific class.)
	template <class T, class E> class TEventHandler : public EventHandler
	{
	public:
		typedef void (T::*HandlerFunctionPtr)(const E&);

		/// Construct with receiver and function pointers and userdata.
		TEventHandler(T* receiver, HandlerFunctionPtr function, void* userData = 0) :
			EventHandler(receiver, userData),
			function_(function)
		{
			assert(receiver_);
			assert(function_);
		}

		/// Invoke event handler function.
		virtual void Invoke(const Event& eventData)
		{
			T* receiver = static_cast<T*>(receiver_);
			(receiver->*function_)((const E&)eventData);
		}
		
	private:
		/// Class-specific pointer to handler function.
		HandlerFunctionPtr function_;
	};

	typedef void(__stdcall *EventFn)(Object* receiver, const void* eventData);

	class StaticEventHandler : public EventHandler
	{
	public:
		/// Construct with receiver and function pointers and userdata.
		StaticEventHandler(Object* receiver, EventFn function) :
			EventHandler(receiver, nullptr)
		{
			function_ = function;
		}

		/// Invoke event handler function.
		virtual void Invoke(const Event& eventData)
		{
			function_(receiver_, &eventData);
		}

	private:
		/// Class-specific pointer to handler function.
		EventFn function_;
	};

	/// Describe an event's hash ID and begin a namespace in which to define its parameters.
#define uEvent(eventName)\
 struct eventName : public Event


}

#endif /* _SRC_EVENT_EVENT_HANDLER_HPP_ */
