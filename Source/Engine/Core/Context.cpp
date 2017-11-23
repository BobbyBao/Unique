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

#include "Precompiled.h"

#include "../Core/Context.h"
//#include "../Core/EventProfiler.h"
#include "../IO/Log.h"
#include "Thread.h"

#ifndef MINI_URHO
//#include <SDL.h>
#ifdef UNIQUE_IK
#include <ik/log.h>
#include <ik/memory.h>
#endif
#endif

#include "../DebugNew.h"

namespace Unique
{

#ifndef MINI_URHO
// Keeps track of how many times SDL was initialised so we know when to call SDL_Quit().
static int sdlInitCounter = 0;

// Keeps track of how many times IK was initialised
static int ikInitCounter = 0;

// Reroute all messages from the ik library to the Unique log
static void HandleIKLog(const char* msg)
{
    UNIQUE_LOGINFOF("[IK] %s", msg);
}
#endif


void EventReceiverGroup::BeginSendEvent()
{
    ++inSend_;
}

void EventReceiverGroup::EndSendEvent()
{
    assert(inSend_ > 0);
    --inSend_;

    if (inSend_ == 0 && dirty_)
    {
        /// \todo Could be optimized by erase-swap, but this keeps the receiver order
        for (unsigned i = receivers_.size() - 1; i < receivers_.size(); --i)
        {
            if (!receivers_[i])
                receivers_.erase(receivers_.begin() + i);
        }

        dirty_ = false;
    }
}

void EventReceiverGroup::Add(Object* object)
{
    if (object)
        receivers_.push_back(object);
}

void EventReceiverGroup::Remove(Object* object)
{
    if (inSend_ > 0)
    {
        PODVector<Object*>::iterator i = Find(receivers_, object);
        if (i != receivers_.end())
        {
            (*i) = 0;
            dirty_ = true;
        }
    }
    else
        Unique::Remove(receivers_, object);
}

static const int kMaxCount = 256;

int s_NumRegisteredCallbacks = 0;
RegisterRuntime::CallbackFunction* s_InitializationCallbacks[kMaxCount];
RegisterRuntime::CallbackFunction* s_CleanupCallbacks[kMaxCount];

RegisterRuntime::RegisterRuntime(RegisterRuntime::CallbackFunction* Initialize, RegisterRuntime::CallbackFunction* Cleanup)
{
	s_InitializationCallbacks[s_NumRegisteredCallbacks] = Initialize;
	s_CleanupCallbacks[s_NumRegisteredCallbacks] = Cleanup;
	s_NumRegisteredCallbacks++;
	assert(s_NumRegisteredCallbacks <= kMaxCount);
}

void ExecuteInitializations(Context* context)
{
	for (int i = 0; i < s_NumRegisteredCallbacks; i++)
	{
		if (s_InitializationCallbacks[i])
			s_InitializationCallbacks[i](context);
	}
}

void ExecuteCleanup(Context* context)
{
	for (int i = s_NumRegisteredCallbacks - 1; i >= 0; i--)
	{
		if (s_CleanupCallbacks[i])
			s_CleanupCallbacks[i](context);
	}
}

Context::Context() :
    eventHandler_(0)
{
#ifdef __ANDROID__
    // Always reset the random seed on Android, as the Unique library might not be unloaded between runs
    SetRandomSeed(1);
#endif
	Object::context_ = this;

    // Set the main thread ID (assuming the Context is created in it)
	Thread::SetMainThread();

	ExecuteInitializations(this);
}

Context::~Context()
{
	for (int i = subsystemVec_.size() - 1; i >=0; i--)
	{
		RemoveSubsystem(subsystemVec_[i]->GetType());
	}
		
	ExecuteCleanup(this);
	Object::context_ = nullptr;
}

SPtr<Object> Context::CreateObject(StringID objectType)
{
	HashMap<StringID, UPtr<ObjectFactory> >::const_iterator i = factories_.find(objectType);
	if (i != factories_.end())
		return i->second->CreateObject();
	else
		return SPtr<Object>();
}

void Context::RegisterFactory(ObjectFactory* factory)
{
	if (!factory)
		return;

	factories_[factory->GetType()] = std::move(UPtr<ObjectFactory>(factory));
}

void Context::RegisterFactory(ObjectFactory* factory, const char* category)
{
	if (!factory)
		return;

	RegisterFactory(factory);

	if (String::CStringLength(category))
		objectCategories_[category].push_back(factory->GetType());
}

void Context::RegisterSubsystem(Object* object)
{
    if (!object)
        return;

	subsystemVec_.push_back(object);
    subsystems_[object->GetType()] = object;
}

void Context::RemoveSubsystem(StringID objectType)
{
    HashMap<StringID, SPtr<Object>>::iterator i = subsystems_.find(objectType);
	if (i != subsystems_.end())
	{
		Remove(subsystemVec_, i->second);
		subsystems_.erase(i);
	}
}

Object* Context::Subsystem(StringID type) const
{
    HashMap<StringID, SPtr<Object>>::const_iterator i = subsystems_.find(type);
    if (i != subsystems_.end())
        return i->second;
    else
        return 0;
}

Object* Context::GetEventSender() const
{
    if (!eventSenders_.empty())
        return eventSenders_.back();
    else
        return 0;
}

void Context::AddEventReceiver(Object* receiver, StringID eventType)
{
    SPtr<EventReceiverGroup>& group = eventReceivers_[eventType];
    if (!group)
        group = new EventReceiverGroup();
    group->Add(receiver);
}

void Context::AddEventReceiver(Object* receiver, Object* sender, StringID eventType)
{
    SPtr<EventReceiverGroup>& group = specificEventReceivers_[sender][eventType];
    if (!group)
        group = new EventReceiverGroup();
    group->Add(receiver);
}

void Context::RemoveEventSender(Object* sender)
{
    HashMap<Object*, HashMap<StringID, SPtr<EventReceiverGroup> > >::iterator i = specificEventReceivers_.find(sender);
    if (i != specificEventReceivers_.end())
    {
        for (HashMap<StringID, SPtr<EventReceiverGroup> >::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
            for (PODVector<Object*>::iterator k = j->second->receivers_.begin(); k != j->second->receivers_.end(); ++k)
            {
                Object* receiver = *k;
                if (receiver)
                    receiver->RemoveEventSender(sender);
            }
        }
        specificEventReceivers_.erase(i);
    }
}

void Context::RemoveEventReceiver(Object* receiver, StringID eventType)
{
    EventReceiverGroup* group = GetEventReceivers(eventType);
    if (group)
        group->Remove(receiver);
}

void Context::RemoveEventReceiver(Object* receiver, Object* sender, StringID eventType)
{
    EventReceiverGroup* group = GetEventReceivers(sender, eventType);
    if (group)
        group->Remove(receiver);
}

void Context::BeginSendEvent(Object* sender, StringID eventType)
{
#ifdef UNIQUE_PROFILING
    if (EventProfiler::IsActive())
    {
        EventProfiler* eventProfiler = Subsystem<EventProfiler>();
        if (eventProfiler)
            eventProfiler->BeginBlock(eventType);
    }
#endif

    eventSenders_.push_back(sender);
}

void Context::EndSendEvent()
{
    eventSenders_.pop_back();

#ifdef UNIQUE_PROFILING
    if (EventProfiler::IsActive())
    {
        EventProfiler* eventProfiler = Subsystem<EventProfiler>();
        if (eventProfiler)
            eventProfiler->EndBlock();
    }
#endif
}

}
