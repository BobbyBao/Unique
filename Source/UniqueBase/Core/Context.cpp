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

Context::Context() :
	eventHandler_(0),
	timeStep_(0.0f),
	timeStepSmoothing_(2),
	minFps_(10),
#if defined(IOS) || defined(TVOS) || defined(__ANDROID__) || defined(__arm__) || defined(__aarch64__)
	maxFps_(60),
	maxInactiveFps_(10),
	pauseMinimized_(true),
#else
	maxFps_(200),
	maxInactiveFps_(60),
	pauseMinimized_(false)
#endif
{
#ifdef __ANDROID__
    // Always reset the random seed on Android, as the Unique library might not be unloaded between runs
    SetRandomSeed(1);
#endif
	context_ = this;

	ExecuteInitializations(this);

	TypeInfo::RegisterTypeInfo();
}

Context::~Context()
{
	for (int i = (int)subsystemVec_.size() - 1; i >=0; i--)
	{
		RemoveSubsystem(subsystemVec_[i]->GetType());
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
    subsystems_[object->GetType()] = object;
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

void Context::ThreadFunction()
{
	// Set the main thread ID (assuming the Context is created in it)
	Thread::SetMainThread();

	Time& timer = RegisterSubsystem<Time>();

	SendEvent(Startup());
	
	frameTimer_.Reset();

	while (shouldRun_)
	{
		timer.BeginFrame(timeStep_);

		{
			UNIQUE_PROFILE(Update);

			Unique::Update eventData;
			eventData.timeStep_ = timeStep_;

			SendEvent(eventData);
			// Logic post-update event
			SendEvent((const PostUpdate&)eventData);
			// Rendering update event
			SendEvent((const RenderUpdate&)eventData);
			// Post-render update event
			SendEvent((const PostRenderUpdate&) eventData);
		}

		ApplyFrameLimit();
		timer.EndFrame();
	}

	SendEvent(Shutdown());
	
}

void Context::ApplyFrameLimit()
{

    unsigned maxFps = maxFps_;

    long long elapsed = 0;

#ifndef __EMSCRIPTEN__
    // Perform waiting loop if maximum FPS set
#if !defined(IOS) && !defined(TVOS)
    if (maxFps)
#else
    // If on iOS/tvOS and target framerate is 60 or above, just let the animation callback handle frame timing
    // instead of waiting ourselves
    if (maxFps < 60)
#endif
    {
        UNIQUE_PROFILE(ApplyFrameLimit);

        long long targetMax = 1000000LL / maxFps;

        for (;;)
        {
            elapsed = frameTimer_.GetUSec(false);
            if (elapsed >= targetMax)
                break;

            // Sleep if 1 ms or more off the frame limiting goal
            if (targetMax - elapsed >= 1000LL)
            {
                unsigned sleepTime = (unsigned)((targetMax - elapsed) / 1000LL);
                Time::Sleep(sleepTime);
            }
        }
    }
#endif

    elapsed = frameTimer_.GetUSec(true);

    // If FPS lower than minimum, clamp elapsed time
    if (minFps_)
    {
        long long targetMin = 1000000LL / minFps_;
        if (elapsed > targetMin)
            elapsed = targetMin;
    }

    // Perform timestep smoothing
    timeStep_ = 0.0f;
    lastTimeSteps_.push_back(elapsed / 1000000.0f);
    if (lastTimeSteps_.size() > timeStepSmoothing_)
    {
        // If the smoothing configuration was changed, ensure correct amount of samples
        lastTimeSteps_.erase(lastTimeSteps_.begin(), lastTimeSteps_.begin() + lastTimeSteps_.size() - timeStepSmoothing_);
        for (unsigned i = 0; i < lastTimeSteps_.size(); ++i)
            timeStep_ += lastTimeSteps_[i];
        timeStep_ /= lastTimeSteps_.size();
    }
    else
        timeStep_ = lastTimeSteps_.back();
}

}
