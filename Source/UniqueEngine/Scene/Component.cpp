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

#include "Core/Context.h"
#include "../Scene/Component.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"
#ifdef UNIQUE_PHYSICS
#include "../Physics/PhysicsWorld.h"
#endif
#ifdef UNIQUE_URHO2D
#include "../Urho2D/PhysicsWorld2D.h"
#endif

//#include "../DebugNew.h"

#ifdef _MSC_VER
#pragma warning(disable:6293)
#endif

namespace Unique
{

const char* autoRemoveModeNames[] = {
    "Disabled",
    "Component",
    "Node",
    0
};

uObject(Component)
{
	uAttribute("ID", id_)
	uAttribute("Enabled", enabled_)
}

Component::Component() :
    node_(0),
    id_(0),
	enabled_(true),
	temporary_(false)
{
}

Component::~Component()
{
}

void Component::GetDependencyNodes(PODVector<Node*>& dest)
{
}

void Component::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
}

void Component::SetEnabled(bool enable)
{
    if (enable != enabled_)
    {
        enabled_ = enable;
        OnSetEnabled();

        // Send change event for the component
        Scene* scene = GetScene();
        if (scene)
        {
			ComponentEnabledChanged eventData;
			eventData.scene_ = scene; 
			eventData.node_ = node_;
			eventData.component_ = this;
            scene->SendEvent(eventData);
        }
    }
}

void Component::Remove()
{
    if (node_)
        node_->RemoveComponent(this);
}

Scene* Component::GetScene() const
{
    return node_ ? node_->GetScene() : 0;
}

void Component::OnNodeSet(Node* node)
{
}

void Component::OnSceneSet(Scene* scene)
{
}

void Component::OnMarkedDirty(Node* node)
{
}

void Component::OnNodeSetEnabled(Node* node)
{
}

void Component::SetID(unsigned id)
{
    id_ = id;
}

void Component::SetNode(Node* node)
{
    node_ = node;
    OnNodeSet(node_);
}

Component* Component::GetComponent(StringID type) const
{
    return node_ ? node_->GetComponent(type) : 0;
}

bool Component::IsEnabledEffective() const
{
    return enabled_ && node_ && node_->IsEnabled();
}

void Component::GetComponents(PODVector<Component*>& dest, StringID type) const
{
    if (node_)
        node_->GetComponents(dest, type);
    else
        dest.clear();
}

Component* Component::GetFixedUpdateSource()
{
    Component* ret = 0;
    Scene* scene = GetScene();

    if (scene)
    {
#ifdef UNIQUE_PHYSICS
        ret = scene->GetComponent<PhysicsWorld>();
#endif
#ifdef UNIQUE_URHO2D
        if (!ret)
            ret = scene->GetComponent<PhysicsWorld2D>();
#endif
    }

    return ret;
}

void Component::DoAutoRemove(AutoRemoveMode mode)
{
    switch (mode)
    {
    case REMOVE_COMPONENT:
        Remove();
        return;

    case REMOVE_NODE:
        if (node_)
            node_->Remove();
        return;

    default:
        return;
    }
}

}
