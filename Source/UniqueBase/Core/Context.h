//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Core/Object.h"
#include "../Core/Thread.h"
#include "../Core/Timer.h"

namespace Unique
{

/// Tracking structure for event receivers.
class UNIQUE_API EventReceiverGroup : public RefCounted
{
public:
    /// Construct.
    EventReceiverGroup() :
        inSend_(0),
        dirty_(false)
    {
    }

    /// Begin event send. When receivers are removed during send, group has to be cleaned up afterward.
    void BeginSendEvent();

    /// End event send. Clean up if necessary.
    void EndSendEvent();

    /// Add receiver. Same receiver must not be double-added!
    void Add(Object* object);

    /// Remove receiver. Leave holes during send, which requires later cleanup.
    void Remove(Object* object);

    /// Receivers. May contain holes during sending.
    PODVector<Object*> receivers_;

private:
    /// "In send" recursion counter.
    unsigned inSend_;
    /// Cleanup required flag.
    bool dirty_;
};

/// Unique execution context. Provides access to subsystems, object factories and attributes, and event receivers.
class UNIQUE_API Context : public Object, public Thread
{
    friend class Object;

public:
    /// Construct.
    Context();
    /// Destruct.
    ~Context();

	/// Create an object by type. Return pointer to it or null if no factory found.
	template <class T> inline SPtr<T> CreateObject()
	{
		return StaticCast<T>(CreateObject(T::GetTypeStatic()));
	}
	/// Create an object by type hash. Return pointer to it or null if no factory found.
	SPtr<Object> CreateObject(const StringID& objectType);
	/// Register a factory for an object type.
	void RegisterFactory(ObjectFactory* factory);
	/// Register a factory for an object type and specify the object category.
	void RegisterFactory(ObjectFactory* factory, const char* category);
    /// Register a subsystem.
    void RegisterSubsystem(Object* subsystem);
    /// Remove a subsystem.
    void RemoveSubsystem(const StringID& objectType);
	
	/// Initialises the specified SDL systems, if not already. Returns true if successful. This call must be matched with ReleaseSDL() when SDL functions are no longer required, even if this call fails.
	bool RequireSDL(unsigned int sdlFlags);
	/// Indicate that you are done with using SDL. Must be called after using RequireSDL().
	void ReleaseSDL();

	/// Template version of registering an object factory.
	template <class T> void RegisterFactory();
	/// Template version of registering an object factory with category.
	template <class T> void RegisterFactory(const char* category);
    /// Template version of removing a subsystem.
    template <class T> void RemoveSubsystem();

    /// Return subsystem by type.
    Object* GetSubsystem(const StringID& type) const;

    /// Return all subsystems.
	const Vector<Object*>& GetSubsystems() const { return subsystemVec_; }

	/// Return all object factories.
	const HashMap<StringID, UPtr<ObjectFactory> >& GetObjectFactories() const { return factories_; }

	/// Return all object categories.
	const HashMap<String, Vector<StringID> >& GetObjectCategories() const { return objectCategories_; }

    /// Return active event sender. Null outside event handling.
    Object* GetEventSender() const;

    /// Return active event handler. Set by Object. Null outside event handling.
    EventHandler* GetEventHandler() const { return eventHandler_; }
	
    /// Return event receivers for a sender and event type, or null if they do not exist.
    EventReceiverGroup* GetEventReceivers(Object* sender, StringID eventType)
    {
        auto i = specificEventReceivers_.find(sender);
        if (i != specificEventReceivers_.end())
        {
            auto j = i->second.find(eventType);
            return j != i->second.end() ? j->second : (EventReceiverGroup*)0;
        }
        else
            return 0;
    }

    /// Return event receivers for an event type, or null if they do not exist.
    EventReceiverGroup* GetEventReceivers(StringID eventType)
    {
        HashMap<StringID, SPtr<EventReceiverGroup> >::iterator i = eventReceivers_.find(eventType);
        return i != eventReceivers_.end() ? i->second : (EventReceiverGroup*)0;
    }

	template <class T>
	class InstanceHolder
	{
		static T*& Instance()
		{
			static T* instance_ = nullptr;
			return instance_;
		}

		friend class Context;
	};

	/// Register a subsystem.
	template<typename T, typename... Rest>
	T& RegisterSubsystem(Rest&... rest)
	{
		T* obj = new T(rest...);
		InstanceHolder<T>::Instance() = obj;
		RegisterSubsystem(obj);
		return *obj;
	}

	template<class T>
	static T* GetSubsystem()
	{
		return InstanceHolder<T>::Instance();
	}

	virtual void ThreadFunction();

private:
    /// Add event receiver.
    void AddEventReceiver(Object* receiver, StringID eventType);
    /// Add event receiver for specific event.
    void AddEventReceiver(Object* receiver, Object* sender, StringID eventType);
    /// Remove an event sender from all receivers. Called on its destruction.
    void RemoveEventSender(Object* sender);
    /// Remove event receiver from specific events.
    void RemoveEventReceiver(Object* receiver, Object* sender, StringID eventType);
    /// Remove event receiver from non-specific events.
    void RemoveEventReceiver(Object* receiver, StringID eventType);
    /// Begin event send.
    void BeginSendEvent(Object* sender, StringID eventType);
    /// End event send. Clean up event receivers removed in the meanwhile.
    void EndSendEvent();

    /// Set current event handler. Called by Object.
    void SetEventHandler(EventHandler* handler) { eventHandler_ = handler; }
	
	void ApplyFrameLimit();

	/// Object factories.
	HashMap<StringID, UPtr<ObjectFactory> > factories_;

	Vector<Object*> subsystemVec_;
    /// Subsystems.
    HashMap<StringID, SPtr<Object>> subsystems_;
    /// Event receivers for non-specific events.
    HashMap<StringID, SPtr<EventReceiverGroup> > eventReceivers_;
    /// Event receivers for specific senders' events.
    HashMap<Object*, HashMap<StringID, SPtr<EventReceiverGroup> > > specificEventReceivers_;
    /// Event sender stack.
    Vector<Object*> eventSenders_;
    /// Active event handler. Not stored in a stack for performance reasons; is needed only in esoteric cases.
    EventHandler* eventHandler_;
	/// Object categories.
	HashMap<String, Vector<StringID> > objectCategories_;
	/// Frame update timer.
	HiresTimer frameTimer_;
	/// Previous timesteps for smoothing.
	PODVector<float> lastTimeSteps_;
	/// Next frame timestep in seconds.
	float timeStep_;
	/// How many frames to average for the smoothed timestep.
	unsigned timeStepSmoothing_;
	/// Minimum frames per second.
	unsigned minFps_;
	/// Maximum frames per second.
	unsigned maxFps_;
	/// Maximum frames per second when the application does not have input focus.
	unsigned maxInactiveFps_;
	/// Pause when minimized flag.
	bool pauseMinimized_;
};

template <class T> void Context::RegisterFactory()
{ RegisterFactory(new ObjectFactoryImpl<T>()); }

template <class T> void Context::RegisterFactory(const char* category)
{ RegisterFactory(new ObjectFactoryImpl<T>(), category); }

template <class T> void Context::RemoveSubsystem()
{ RemoveSubsystem(T::GetTypeStatic()); }

class RegisterRuntime
{
public:
	typedef void CallbackFn(Context* context);
	RegisterRuntime(CallbackFn* initialize, CallbackFn* cleanup = nullptr);
};


UNIQUE_C_API Context* GetContext();
template <class T> T& GetSubsystem() { return *GetContext()->GetSubsystem<T>(); }
template <class T> bool HasSubsystem() { return GetContext()->GetSubsystem<T>() != nullptr; }


}
