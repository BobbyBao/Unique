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

#include "Core/Object.h"

namespace Unique
{

/// Variable timestep scene update.
uEvent(SceneUpdate)
{
    Scene* scene_;
    float timeStep_;
};

/// scene_ subsystem update.
uEvent(SceneSubsystemUpdate)
{
    Scene* scene_;
    float timeStep_;
};

/// scene_ transform smoothing update.
uEvent(UpdateSmoothing)
{
    float constant_;
    float squaredSnapThreshold_;
};

/// scene_ drawable update finished. Custom animation (eg. IK) can be done at this point.
uEvent(SceneDrawableUpdateFinished)
{
    Scene* scene_;
    float timeStep_;
};

/// SmoothedTransform target position changed.
uEvent(TargetPositionChanged)
{
};

/// SmoothedTransform target position changed.
uEvent(TargetRotationChanged)
{
};

/// scene_ attribute animation update.
uEvent(AttributeAnimationUpdate)
{
    Scene* scene_;
    float timeStep_;
};

/// Variable timestep scene post-update.
uEvent(ScenePostUpdate)
{
    Scene* scene_;
    float timeStep_;
};

/// Asynchronous scene loading progress.
uEvent(AsyncLoadProgress)
{
    Scene* scene_;
    float Progress;
    int LoadedNodes;
    int TotalNodes;
    int LoadedResources;
    int TotalResources;
};

/// Asynchronous scene loading finished.
uEvent(AsyncLoadFinished)
{
    Scene* scene_;
};

/// A child node has been added to a parent node.
uEvent(NodeAdded)
{
    Scene* scene_;
    Node* parent_;
    Node* node_;
};

/// A child node is about to be removed from a parent node. Note that individual component removal events will not be sent.
uEvent(NodeRemoved)
{
    Scene* scene_;
    Node* parent_;
    Node* node_;
};

/// A component has been created to a node.
uEvent(ComponentAdded)
{
    Scene* scene_;
    Node* node_;
    Component* component_;
};

/// A component is about to be removed from a node.
uEvent(ComponentRemoved)
{
    Scene* scene_;
    Node* node_;
    Component* component_;
};

/// A node's name has changed.
uEvent(NodeNameChanged)
{
    Scene* scene_;
    Node* node_;
};

/// A node's enabled state has changed.
uEvent(NodeEnabledChanged)
{
    Scene* scene_;
    Node* node_;
};

/// A node's tag has been added.
uEvent(NodeTagAdded)
{
    Scene* scene_;
    Node* node_;
    String tag_;
};

/// A node's tag has been removed.
uEvent(NodeTagRemoved)
{
    Scene* scene_;
    Node* node_;
    String tag_;
};

/// A component's enabled state has changed.
uEvent(ComponentEnabledChanged)
{
    Scene* scene_;
    Node* node_;
    Component* component_;
};

/// A node (and its children and components) has been cloned.
uEvent(NodeCloned)
{
    Scene* scene_;
    Node* node_;
    Node* cloneNode_;
};

/// A component has been cloned.
uEvent(ComponentCloned)
{
    Scene* scene_;
    Component* component_;
    Component* cloneComponent_;
};

/// Physics world is about to be stepped.
uEvent(PhysicsPreStep)
{
	class PhysicsWorld* world_;
	float timeStep_;
};

/// Physics world has been stepped.
uEvent(PhysicsPostStep)
{
	class PhysicsWorld* world_;
	float timeStep_;
};

}
