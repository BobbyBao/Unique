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
    Scene* scene_;              // scene_ pointer
    float timeStep_;            // float
};

/// scene_ subsystem update.
uEvent(SceneSubsystemUpdate)
{
    Scene* scene_;                  // scene_ pointer
    float timeStep_;            // float
};

/// scene_ transform smoothing update.
uEvent(UpdateSmoothing)
{
    float constant_;            // float
    float squaredSnapThreshold_;  // float
};

/// scene_ drawable update finished. Custom animation (eg. IK) can be done at this point.
uEvent(SceneDrawableUpdateFinished)
{
    Scene* scene_;                  // scene_ pointer
    float timeStep_;            // float
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
    Scene* scene_;                  // scene_ pointer
    float timeStep_;            // float
};

/// Variable timestep scene post-update.
uEvent(ScenePostUpdate)
{
    Scene* scene_;                  // scene_ pointer
    float timeStep_;            // float
};

/// Asynchronous scene loading progress.
uEvent(AsyncLoadProgress)
{
    Scene* scene_;                  // scene_ pointer
    float Progress;            // float
    int LoadedNodes;      // int
    int TotalNodes;        // int
    int LoadedResources; // int
    int TotalResources;   // int
};

/// Asynchronous scene loading finished.
uEvent(AsyncLoadFinished)
{
    Scene* scene_;                  // scene_ pointer
};

/// A child node has been added to a parent node.
uEvent(NodeAdded)
{
    Scene* scene_;                  // scene_ pointer
    Node* parent_;                // Node pointer
    Node* node_;                    // Node pointer
};

/// A child node is about to be removed from a parent node. Note that individual component removal events will not be sent.
uEvent(NodeRemoved)
{
    Scene* scene_;                  // scene_ pointer
    Node* parent_;                // Node pointer
    Node* node_;                    // Node pointer
};

/// A component has been created to a node.
uEvent(ComponentAdded)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
    Component* component_;          // Component pointer
};

/// A component is about to be removed from a node.
uEvent(ComponentRemoved)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
    Component* component_;          // Component pointer
};

/// A node's name has changed.
uEvent(NodeNameChanged)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
};

/// A node's enabled state has changed.
uEvent(NodeEnabledChanged)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
};

/// A node's tag has been added.
uEvent(NodeTagAdded)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
    String tag_;                      // String tag
};

/// A node's tag has been removed.
uEvent(NodeTagRemoved)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
    String tag_;                      // String tag
};

/// A component's enabled state has changed.
uEvent(ComponentEnabledChanged)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
    Component* component_;          // Component pointer
};

/// A node (and its children and components) has been cloned.
uEvent(NodeCloned)
{
    Scene* scene_;                  // scene_ pointer
    Node* node_;                    // Node pointer
    Node* cloneNode_;          // Node pointer
};

/// A component has been cloned.
uEvent(ComponentCloned)
{
    Scene* scene_;                  // scene_ pointer
    Component* component_;          // Component pointer
    Component* cloneComponent_; // Component pointer
};


}
