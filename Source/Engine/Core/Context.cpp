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
#include "CoreEvents.h"
#include "Thread.h"
#include "Timer.h"
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
	RegisterRuntime::CallbackFunction* sInitializationCallbacks[MAX_RUNTIME_CALLBACK];
	RegisterRuntime::CallbackFunction* sCleanupCallbacks[MAX_RUNTIME_CALLBACK];
}

RegisterRuntime::RegisterRuntime(RegisterRuntime::CallbackFunction* Initialize, RegisterRuntime::CallbackFunction* Cleanup)
{
	sInitializationCallbacks[sNumRegisteredCallbacks] = Initialize;
	sCleanupCallbacks[sNumRegisteredCallbacks] = Cleanup;
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
	Object::context_ = this;

    // Set the main thread ID (assuming the Context is created in it)
	Thread::SetMainThread();

	ExecuteInitializations(this);
}

Context::~Context()
{
	for (int i = (int)subsystemVec_.size() - 1; i >=0; i--)
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

void Context::ThreadFunction()
{
	Time& timer = RegisterSubsystem<Time>();

	SendEvent(E_INIT, Init());
	
	frameTimer_.Reset();

	while (shouldRun_)
	{
		timer.BeginFrame(0.05f);

		{
			UNIQUE_PROFILE(Update);

			Unique::Update eventData;
			eventData.timeStep_ = timeStep_;

			SendEvent(E_UPDATE, eventData);

			// Logic post-update event
			SendEvent(E_POSTUPDATE, eventData);

			// Rendering update event
			SendEvent(E_RENDERUPDATE, eventData);

			// Post-render update event
			SendEvent(E_POSTRENDERUPDATE, eventData);
		}

		ApplyFrameLimit();
		timer.EndFrame();
	}

	SendEvent(E_SHUTDOWN, Shutdown());
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
#ifdef UNIQUE_TESTING
    if (timeOut_ > 0)
    {
        timeOut_ -= elapsed;
        if (timeOut_ <= 0)
            Exit();
    }
#endif

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
