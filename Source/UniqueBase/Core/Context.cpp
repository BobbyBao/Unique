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
#include "../IO/Log.h"
#include "CoreEvents.h"
#include "Thread.h"
#include "Timer.h"
#include <SDL/SDL.h>
#include "../DebugNew.h"

namespace Unique
{

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
        for (size_t i = receivers_.size() - 1; i < receivers_.size(); --i)
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


namespace
{
	const int MAX_RUNTIME_CALLBACK = 256;

	int sNumRegisteredCallbacks = 0;
	RegisterRuntime::CallbackFn* sInitializationCallbacks[MAX_RUNTIME_CALLBACK];
	RegisterRuntime::CallbackFn* sCleanupCallbacks[MAX_RUNTIME_CALLBACK];

	// Keeps track of how many times SDL was initialised so we know when to call SDL_Quit().
	static int sdlInitCounter = 0;

}

RegisterRuntime::RegisterRuntime(RegisterRuntime::CallbackFn* initialize, RegisterRuntime::CallbackFn* cleanup)
{
	sInitializationCallbacks[sNumRegisteredCallbacks] = initialize;
	sCleanupCallbacks[sNumRegisteredCallbacks] = cleanup;
	sNumRegisteredCallbacks++;
	assert(sNumRegisteredCallbacks <= MAX_RUNTIME_CALLBACK);
}

void ExecuteInitializations(Context* context)
{
	for (int i = 0; i < sNumRegisteredCallbacks; i++)
	{
		if (sInitializationCallbacks[i])
			sInitializationCallbacks[i](context);
	}
}

void ExecuteCleanup(Context* context)
{
	for (int i = sNumRegisteredCallbacks - 1; i >= 0; i--)
	{
		if (sCleanupCallbacks[i])
			sCleanupCallbacks[i](context);
	}
}

/// Execution context.
static Context* context_;

UNIQUE_C_API Context* GetContext()
{
	return context_;
}

Context::Context() : eventHandler_(0)
{
#ifdef __ANDROID__
    // Always reset the random seed on Android, as the Unique library might not be unloaded between runs
    SetRandomSeed(1);
#endif
	context_ = this;

	ExecuteInitializations(this);

	TypeInfo::RegisterTypeInfo();

	RegisterSubsystem<Time>();
}

Context::~Context()
{
	while(!subsystemVec_.empty())
	{
		RemoveSubsystem(subsystemVec_.back()->GetType());
	}
		
	ExecuteCleanup(this);

	context_ = nullptr;
}

SPtr<Object> Context::CreateObject(const StringID& objectType)
{
	auto i = factories_.find(objectType);
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
    subsystems_[object->GetType()] = SPtr<Object>(object);
}

void Context::RemoveSubsystem(const StringID& objectType)
{
    auto i = subsystems_.find(objectType);
	if (i != subsystems_.end())
	{
		Remove(subsystemVec_, i->second);
		subsystems_.erase(i);
	}
}

Object* Context::GetSubsystem(const StringID& type) const
{
    auto i = subsystems_.find(type);
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
    eventSenders_.push_back(sender);
}

void Context::EndSendEvent()
{
    eventSenders_.pop_back();
}

bool Context::RequireSDL(unsigned int sdlFlags)
{
	// Always increment, the caller must match with ReleaseSDL(), regardless of
	// what happens.
	++sdlInitCounter;

	// Need to call SDL_Init() at least once before SDL_InitSubsystem()
	if (sdlInitCounter == 1)
	{
		UNIQUE_LOGDEBUG("Initialising SDL");
		if (SDL_Init(0) != 0)
		{
			UNIQUE_LOGERRORF("Failed to initialise SDL: %s", SDL_GetError());
			return false;
		}
	}

	Uint32 remainingFlags = sdlFlags & ~SDL_WasInit(0);
	if (remainingFlags != 0)
	{
		if (SDL_InitSubSystem(remainingFlags) != 0)
		{
			UNIQUE_LOGERRORF("Failed to initialise SDL subsystem: %s", SDL_GetError());
			return false;
		}
	}

	return true;
}

void Context::ReleaseSDL()
{
	--sdlInitCounter;

	if (sdlInitCounter == 0)
	{
		UNIQUE_LOGDEBUG("Quitting SDL");
		SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
		SDL_Quit();
	}

	if (sdlInitCounter < 0)
		UNIQUE_LOGERROR("Too many calls to Context::ReleaseSDL()!");
}

}
