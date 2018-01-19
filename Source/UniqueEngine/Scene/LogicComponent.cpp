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

#include "IO/Log.h"
#if defined(UNIQUE_PHYSICS)
#include "../Physics/PhysicsEvents.h"
#endif
#include "../Scene/LogicComponent.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"

namespace Unique
{

LogicComponent::LogicComponent() :
    updateEventMask_(USE_UPDATE | USE_POSTUPDATE | USE_FIXEDUPDATE | USE_FIXEDPOSTUPDATE),
    currentEventMask_(0),
    delayedStartCalled_(false)
{
}

LogicComponent::~LogicComponent()
{
}

void LogicComponent::OnSetEnabled()
{
    UpdateEventSubscription();
}

void LogicComponent::Update(float timeStep)
{
}

void LogicComponent::PostUpdate(float timeStep)
{
}

void LogicComponent::FixedUpdate(float timeStep)
{
}

void LogicComponent::FixedPostUpdate(float timeStep)
{
}

void LogicComponent::SetUpdateEventMask(unsigned char mask)
{
    if (updateEventMask_ != mask)
    {
        updateEventMask_ = mask;
        UpdateEventSubscription();
    }
}

void LogicComponent::OnNodeSet(Node* node)
{
    if (node)
    {
        // Execute the user-defined start function
        Start();
    }
    else
    {
        // We are being detached from a node: execute user-defined stop function and prepare for destruction
        Stop();
    }
}

void LogicComponent::OnSceneSet(Scene* scene)
{
    if (scene)
        UpdateEventSubscription();
    else
    {
        Unsubscribe<SceneUpdate>();
        Unsubscribe<ScenePostUpdate>();
#if defined(UNIQUE_PHYSICS)
        Unsubscribe(E_PHYSICSPRESTEP);
        Unsubscribe(E_PHYSICSPOSTSTEP);
#endif
        currentEventMask_ = 0;
    }
}

void LogicComponent::UpdateEventSubscription()
{
    Scene* scene = GetScene();
    if (!scene)
        return;

    bool enabled = IsEnabledEffective();

    bool needUpdate = enabled && ((updateEventMask_ & USE_UPDATE) || !delayedStartCalled_);
    if (needUpdate && !(currentEventMask_ & USE_UPDATE))
    {
		SubscribeTo(scene, &LogicComponent::HandleSceneUpdate);
        currentEventMask_ |= USE_UPDATE;
    }
    else if (!needUpdate && (currentEventMask_ & USE_UPDATE))
    {
        UnsubscribeFrom<SceneUpdate>(scene);
        currentEventMask_ &= ~USE_UPDATE;
    }

    bool needPostUpdate = enabled && (updateEventMask_ & USE_POSTUPDATE);
    if (needPostUpdate && !(currentEventMask_ & USE_POSTUPDATE))
    {
		SubscribeTo(scene, &LogicComponent::HandleScenePostUpdate);
        currentEventMask_ |= USE_POSTUPDATE;
    }
    else if (!needPostUpdate && (currentEventMask_ & USE_POSTUPDATE))
    {
        UnsubscribeFrom<ScenePostUpdate>(scene);
        currentEventMask_ &= ~USE_POSTUPDATE;
    }

    Component* world = GetFixedUpdateSource();
    if (!world)
        return;

    bool needFixedUpdate = enabled && (updateEventMask_ & USE_FIXEDUPDATE);
    if (needFixedUpdate && !(currentEventMask_ & USE_FIXEDUPDATE))
    {
        SubscribeTo(world, &LogicComponent::HandlePhysicsPreStep);
        currentEventMask_ |= USE_FIXEDUPDATE;
    }
    else if (!needFixedUpdate && (currentEventMask_ & USE_FIXEDUPDATE))
    {
        UnsubscribeFrom<PhysicsPreStep>(world);
        currentEventMask_ &= ~USE_FIXEDUPDATE;
    }

    bool needFixedPostUpdate = enabled && (updateEventMask_ & USE_FIXEDPOSTUPDATE);
    if (needFixedPostUpdate && !(currentEventMask_ & USE_FIXEDPOSTUPDATE))
    {
        SubscribeTo(world, &LogicComponent::HandlePhysicsPostStep);
        currentEventMask_ |= USE_FIXEDPOSTUPDATE;
    }
    else if (!needFixedPostUpdate && (currentEventMask_ & USE_FIXEDPOSTUPDATE))
    {
        UnsubscribeFrom<PhysicsPostStep>(world);
        currentEventMask_ &= ~USE_FIXEDPOSTUPDATE;
    }

}

void LogicComponent::HandleSceneUpdate(const SceneUpdate& eventData)
{
    // Execute user-defined delayed start function before first update
    if (!delayedStartCalled_)
    {
        DelayedStart();
        delayedStartCalled_ = true;

        // If did not need actual update events, unsubscribe now
        if (!(updateEventMask_ & USE_UPDATE))
        {
            UnsubscribeFrom<SceneUpdate>(GetScene());
            currentEventMask_ &= ~USE_UPDATE;
            return;
        }
    }

    // Then execute user-defined update function
    Update(eventData.timeStep_);
}

void LogicComponent::HandleScenePostUpdate(const ScenePostUpdate& eventData)
{
    // Execute user-defined post-update function
	PostUpdate(eventData.timeStep_);
}

void LogicComponent::HandlePhysicsPreStep(const struct PhysicsPreStep& eventData)
{
    // Execute user-defined delayed start function before first fixed update if not called yet
    if (!delayedStartCalled_)
    {
        DelayedStart();
        delayedStartCalled_ = true;
    }

    // Execute user-defined fixed update function
    FixedUpdate(eventData.timeStep_);
}

void LogicComponent::HandlePhysicsPostStep(const struct PhysicsPostStep& eventData)
{
    // Execute user-defined fixed post-update function
    FixedPostUpdate(eventData.timeStep_);
}


}
