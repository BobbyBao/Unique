#include "Precompiled.h"

#include "Core/Context.h"
#include "Core/Profiler.h"
#include "IO/Log.h"
#include "../Scene/Component.h"
//#include "../Scene/ObjectAnimation.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"
#include "../Scene/SmoothedTransform.h"
//#include "../Scene/UnknownComponent.h"

#include "DebugNew.h"

#ifdef _MSC_VER
#pragma warning(disable:6293)
#endif

namespace Unique
{

	uObject(Node)
	{
		uAccessor("Is Enabled", IsEnabled, SetEnabled, true);
		uAccessor("Name", GetName, SetName, "");
		uAccessor("Tags", GetTags, SetTags, StringVector());
		uAccessor("Position", GetPosition, SetPosition, Vector3::ZERO);
		uAccessor("Rotation", GetRotation, SetRotation, Quaternion::IDENTITY);
		uAccessor("Scale", GetScale, SetScale, Vector3::ONE);
		uAttribute("Components", components_);
		uAttribute("Children", children_);
	}

Node::Node() :
    worldTransform_(Matrix3x4::IDENTITY),
    dirty_(false),
    enabled_(true),
    enabledPrev_(true),
    parent_(0),
    scene_(0),
    id_(0),
    position_(Vector3::ZERO),
    rotation_(Quaternion::IDENTITY),
    scale_(Vector3::ONE),
    worldRotation_(Quaternion::IDENTITY),
	temporary_(false)
{
}

Node::~Node()
{
    RemoveAllChildren();
    RemoveAllComponents();

    // Remove from the scene
    if (scene_)
        scene_->NodeRemoved(this);
}

void Node::SetName(const StringID& name)
{
    if (name != name_)
    {
        name_ = name;

        // Send change event
        if (scene_)
        {
			NodeNameChanged eventData;
			eventData.scene_ = scene_;
			eventData.node_ = this;
            scene_->SendEvent(eventData);
        }
    }
}

void Node::SetTags(const StringVector& tags)
{
    RemoveAllTags();
    AddTags(tags);
}

void Node::AddTag(const String& tag)
{
    // Check if tag empty or already added
    if (tag.Empty() || HasTag(tag))
        return;

    // Add tag
    tags_.push_back(tag);

    // Cache
    scene_->NodeTagAdded(this, tag);

	NodeTagAdded eventData;
	eventData.scene_ = scene_;
	eventData.node_ = this;
	eventData.tag_ = tag;
    scene_->SendEvent(eventData);

}

void Node::AddTags(const String& tags, char separator)
{
    StringVector tagVector = tags.Split(separator);
    AddTags(tagVector);
}

void Node::AddTags(const StringVector& tags)
{
    // This is OK, as MarkNetworkUpdate() early-outs when called multiple times
    for (unsigned i = 0; i < tags.size(); ++i)
        AddTag(tags[i]);
}

bool Node::RemoveTag(const String& tag)
{
    bool removed = Unique::Remove(tags_, tag);

    // Nothing to do
    if (!removed)
        return false;

    // Scene cache update
    if (scene_)
    {
        scene_->NodeTagRemoved(this, tag);

		NodeTagRemoved eventData;
		eventData.scene_ = scene_;
		eventData.node_ = this;
		eventData.tag_ = tag;
        scene_->SendEvent(eventData);
    }

    return true;
}

void Node::RemoveAllTags()
{
    // Clear old scene cache
    if (scene_)
    {
        for (unsigned i = 0; i < tags_.size(); ++i)
        {
            scene_->NodeTagRemoved(this, tags_[i]);

			NodeTagRemoved eventData;
			eventData.scene_ = scene_;
			eventData.node_ = this;
			eventData.tag_ = tags_[i];
            scene_->SendEvent(eventData);
        }
    }

    tags_.clear();

}

void Node::SetPosition(const Vector3& position)
{
    position_ = position;
    MarkDirty();

}

void Node::SetRotation(const Quaternion& rotation)
{
    rotation_ = rotation;
    MarkDirty();

}

void Node::SetDirection(const Vector3& direction)
{
    SetRotation(Quaternion(Vector3::FORWARD, direction));
}

void Node::SetScale(const Vector3& scale)
{
    scale_ = scale;
    // Prevent exact zero scale e.g. from momentary edits as this may cause division by zero
    // when decomposing the world transform matrix
    if (scale_.x_ == 0.0f)
        scale_.x_ = M_EPSILON;
    if (scale_.y_ == 0.0f)
        scale_.y_ = M_EPSILON;
    if (scale_.z_ == 0.0f)
        scale_.z_ = M_EPSILON;

    MarkDirty();
}

void Node::SetTransform(const Vector3& position, const Quaternion& rotation)
{
    position_ = position;
    rotation_ = rotation;
    MarkDirty();

}

void Node::SetTransform(const Vector3& position, const Quaternion& rotation, float scale)
{
    SetTransform(position, rotation, Vector3(scale, scale, scale));
}

void Node::SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
    position_ = position;
    rotation_ = rotation;
    scale_ = scale;
    MarkDirty();

}

void Node::SetTransform(const Matrix3x4& matrix)
{
    SetTransform(matrix.Translation(), matrix.Rotation(), matrix.Scale());
}

void Node::SetWorldPosition(const Vector3& position)
{
    SetPosition((parent_ == scene_ || !parent_) ? position : parent_->GetWorldTransform().Inverse() * position);
}

void Node::SetWorldRotation(const Quaternion& rotation)
{
    SetRotation((parent_ == scene_ || !parent_) ? rotation : parent_->GetWorldRotation().Inverse() * rotation);
}

void Node::SetWorldDirection(const Vector3& direction)
{
    Vector3 localDirection = (parent_ == scene_ || !parent_) ? direction : parent_->GetWorldRotation().Inverse() * direction;
    SetRotation(Quaternion(Vector3::FORWARD, localDirection));
}

void Node::SetWorldScale(float scale)
{
    SetWorldScale(Vector3(scale, scale, scale));
}

void Node::SetWorldScale(const Vector3& scale)
{
    SetScale((parent_ == scene_ || !parent_) ? scale : scale / parent_->GetWorldScale());
}

void Node::SetWorldTransform(const Vector3& position, const Quaternion& rotation)
{
    SetWorldPosition(position);
    SetWorldRotation(rotation);
}

void Node::SetWorldTransform(const Vector3& position, const Quaternion& rotation, float scale)
{
    SetWorldPosition(position);
    SetWorldRotation(rotation);
    SetWorldScale(scale);
}

void Node::SetWorldTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
    SetWorldPosition(position);
    SetWorldRotation(rotation);
    SetWorldScale(scale);
}

void Node::Translate(const Vector3& delta, TransformSpace space)
{
    switch (space)
    {
    case TS_LOCAL:
        // Note: local space translation disregards local scale for scale-independent movement speed
        position_ += rotation_ * delta;
        break;

    case TS_PARENT:
        position_ += delta;
        break;

    case TS_WORLD:
        position_ += (parent_ == scene_ || !parent_) ? delta : parent_->GetWorldTransform().Inverse() * delta;
        break;
    }

    MarkDirty();

}

void Node::Rotate(const Quaternion& delta, TransformSpace space)
{
    switch (space)
    {
    case TS_LOCAL:
        rotation_ = (rotation_ * delta).Normalized();
        break;

    case TS_PARENT:
        rotation_ = (delta * rotation_).Normalized();
        break;

    case TS_WORLD:
        if (parent_ == scene_ || !parent_)
            rotation_ = (delta * rotation_).Normalized();
        else
        {
            Quaternion worldRotation = GetWorldRotation();
            rotation_ = rotation_ * worldRotation.Inverse() * delta * worldRotation;
        }
        break;
    }

    MarkDirty();

}

void Node::RotateAround(const Vector3& point, const Quaternion& delta, TransformSpace space)
{
    Vector3 parentSpacePoint;
    Quaternion oldRotation = rotation_;

    switch (space)
    {
    case TS_LOCAL:
        parentSpacePoint = GetTransform() * point;
        rotation_ = (rotation_ * delta).Normalized();
        break;

    case TS_PARENT:
        parentSpacePoint = point;
        rotation_ = (delta * rotation_).Normalized();
        break;

    case TS_WORLD:
        if (parent_ == scene_ || !parent_)
        {
            parentSpacePoint = point;
            rotation_ = (delta * rotation_).Normalized();
        }
        else
        {
            parentSpacePoint = parent_->GetWorldTransform().Inverse() * point;
            Quaternion worldRotation = GetWorldRotation();
            rotation_ = rotation_ * worldRotation.Inverse() * delta * worldRotation;
        }
        break;
    }

    Vector3 oldRelativePos = oldRotation.Inverse() * (position_ - parentSpacePoint);
    position_ = rotation_ * oldRelativePos + parentSpacePoint;

    MarkDirty();

}

void Node::Yaw(float angle, TransformSpace space)
{
    Rotate(Quaternion(angle, Vector3::UP), space);
}

void Node::Pitch(float angle, TransformSpace space)
{
    Rotate(Quaternion(angle, Vector3::RIGHT), space);
}

void Node::Roll(float angle, TransformSpace space)
{
    Rotate(Quaternion(angle, Vector3::FORWARD), space);
}

bool Node::LookAt(const Vector3& target, const Vector3& up, TransformSpace space)
{
    Vector3 worldSpaceTarget;

    switch (space)
    {
    case TS_LOCAL:
        worldSpaceTarget = GetWorldTransform() * target;
        break;

    case TS_PARENT:
        worldSpaceTarget = (parent_ == scene_ || !parent_) ? target : parent_->GetWorldTransform() * target;
        break;

    case TS_WORLD:
        worldSpaceTarget = target;
        break;
    }

    Vector3 lookDir = worldSpaceTarget - GetWorldPosition();
    // Check if target is very close, in that case can not reliably calculate lookat direction
    if (lookDir.Equals(Vector3::ZERO))
        return false;
    Quaternion newRotation;
    // Do nothing if setting look rotation failed
    if (!newRotation.FromLookRotation(lookDir, up))
        return false;

    SetWorldRotation(newRotation);
    return true;
}

void Node::Scale(float scale)
{
    Scale(Vector3(scale, scale, scale));
}

void Node::Scale(const Vector3& scale)
{
    scale_ *= scale;
    MarkDirty();

}

void Node::SetEnabled(bool enable)
{
	SetEnabledImpl(enable, false, true);
}

void Node::SetDeepEnabled(bool enable)
{
	SetEnabledImpl(enable, true, false);
}

void Node::ResetDeepEnabled()
{
	SetEnabledImpl(enabledPrev_, false, false);

    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
        (*i)->ResetDeepEnabled();
}

void Node::SetEnabledRecursive(bool enable)
{
	SetEnabledImpl(enable, true, true);
}

void Node::MarkDirty()
{
    Node *cur = this;
    for (;;)
    {
        // Precondition:
        // a) whenever a node is marked dirty, all its children are marked dirty as well.
        // b) whenever a node is cleared from being dirty, all its parents must have been
        //    cleared as well.
        // Therefore if we are recursing here to mark this node dirty, and it already was,
        // then all children of this node must also be already dirty, and we don't need to
        // reflag them again.
        if (cur->dirty_)
            return;
        cur->dirty_ = true;

        // Notify listener components first, then mark child nodes
        for (Vector<WPtr<Component> >::iterator i = cur->listeners_.begin(); i != cur->listeners_.end();)
        {
            Component *c = *i;
            if (c)
            {
                c->OnMarkedDirty(cur);
                ++i;
            }
            // If listener has expired, erase from list (swap with the last element to avoid O(n^2) behavior)
            else
            {
                *i = cur->listeners_.back();
                cur->listeners_.pop_back();
            }
        }

        // Tail call optimization: Don't recurse to mark the first child dirty, but
        // instead process it in the context of the current function. If there are more
        // than one child, then recurse to the excess children.
        Vector<SPtr<Node> >::iterator i = cur->children_.begin();
        if (i != cur->children_.end())
        {
            Node *next = *i;
            for (++i; i != cur->children_.end(); ++i)
                (*i)->MarkDirty();
            cur = next;
        }
        else
            return;
    }
}

Node* Node::CreateChild(const StringID& name, unsigned id, bool temporary)
{
    Node* newNode = CreateChild(id, temporary);
    newNode->SetName(name);
    return newNode;
}

Node* Node::CreateTemporaryChild(const StringID& name, unsigned id)
{
    return CreateChild(name, id, true);
}

void Node::AddChild(Node* node, unsigned index)
{
    // Check for illegal or redundant parent assignment
    if (!node || node == this || node->parent_ == this)
        return;
    // Check for possible cyclic parent assignment
    if (IsChildOf(node))
        return;

    // Keep a shared ptr to the node while tansferring
    SPtr<Node> nodeShared(node);
    Node* oldParent = node->parent_;
    if (oldParent)
    {
        // If old parent is in different scene, perform the full removal
        if (oldParent->GetScene() != scene_)
            oldParent->RemoveChild(node);
        else
        {
            if (scene_)
            {
                // Otherwise do not remove from the scene during reparenting, just send the necessary change event
				NodeRemoved eventData;
				eventData.scene_ = scene_;
				eventData.parent_ = oldParent;
				eventData.node_ = node;
                scene_->SendEvent(eventData);
            }

            Unique::Remove(oldParent->children_, nodeShared);
        }
    }

	auto it = index >= children_.size() ? children_.end() : children_.begin() + index;
    // Add to the child vector, then add to the scene if not added yet
    children_.insert(it, nodeShared);
    if (scene_ && node->GetScene() != scene_)
        scene_->NodeAdded(node);

    node->parent_ = this;
    node->MarkDirty();

    // Send change event
    if (scene_)
    {
		NodeAdded eventData;
		eventData.scene_ = scene_;
		eventData.parent_ = this;
		eventData.node_ = node;
        scene_->SendEvent(eventData);
    }
}

void Node::RemoveChild(Node* node)
{
    if (!node)
        return;

    for (Vector<SPtr<Node> >::iterator i = children_.begin(); i != children_.end(); ++i)
    {
        if (*i == node)
        {
            RemoveChild(i);
            return;
        }
    }
}

void Node::RemoveAllChildren()
{
    RemoveChildren(true);
}

void Node::RemoveChildren(bool recursive)
{
    unsigned numRemoved = 0;

    for (size_t i = children_.size() - 1; i < children_.size(); --i)
    {
        Node* childNode = children_[i];

        if (recursive)
            childNode->RemoveChildren(true);

		RemoveChild(children_.begin() + i);
		++numRemoved;        
    }

}

Component* Node::CreateComponent(StringID type, unsigned id)
{
    // Check that creation succeeds and that the object in fact is a component
    SPtr<Component> newComponent = DynamicCast<Component>(GetContext()->CreateObject(type));
    if (!newComponent)
    {
        UNIQUE_LOGERROR("Could not create unknown component type " + type.ToString());
        return 0;
    }

    AddComponent(newComponent, id);
    return newComponent;
}

Component* Node::GetOrCreateComponent(StringID type, unsigned id)
{
    Component* oldComponent = GetComponent(type);
    if (oldComponent)
        return oldComponent;
    else
        return CreateComponent(type, id);
}

Component* Node::CloneComponent(Component* component, unsigned id)
{
    if (!component)
    {
        UNIQUE_LOGERROR("Null source component given for CloneComponent");
        return 0;
    }

    Component* cloneComponent = CreateComponent(component->GetType(), 0);
    if (!cloneComponent)
    {
        UNIQUE_LOGERROR("Could not clone component " + component->GetType().ToString());
        return 0;
    }
	/*
    const Vector<AttributeInfo>* compAttributes = component->GetAttributes();
    const Vector<AttributeInfo>* cloneAttributes = cloneComponent->GetAttributes();

    if (compAttributes)
    {
        for (unsigned i = 0; i < compAttributes->size() && i < cloneAttributes->size(); ++i)
        {
            const AttributeInfo& attr = compAttributes->at(i);
            const AttributeInfo& cloneAttr = cloneAttributes->at(i);
            if (attr.mode_ & TF_DEFAULT)
            {
                Variant value;
                component->OnGetAttribute(attr, value);
                // Note: when eg. a ScriptInstance component is cloned, its script object attributes are unique and therefore we
                // can not simply refer to the source component's AttributeInfo
                cloneComponent->OnSetAttribute(cloneAttr, value);
            }
        }
        cloneComponent->ApplyAttributes();
    }

    {
        using namespace ComponentCloned;

        VariantMap& eventData = GetEventDataMap();
        eventData[P_SCENE] = scene_;
        eventData[P_COMPONENT] = component;
        eventData[P_CLONECOMPONENT] = cloneComponent;

        scene_->SendEvent(E_COMPONENTCLONED, eventData);
    }
	*/
    return cloneComponent;
}

void Node::RemoveComponent(Component* component)
{
    for (Vector<SPtr<Component> >::iterator i = components_.begin(); i != components_.end(); ++i)
    {
        if (*i == component)
        {
            RemoveComponent(i);

            return;
        }
    }
}

void Node::RemoveComponent(StringID type)
{
    for (Vector<SPtr<Component> >::iterator i = components_.begin(); i != components_.end(); ++i)
    {
        if ((*i)->GetType() == type)
        {
            RemoveComponent(i);

            return;
        }
    }
}

void Node::RemoveComponents()
{
    unsigned numRemoved = 0;

    for (size_t i = components_.size() - 1; i < components_.size(); --i)
    {
		RemoveComponent(components_.begin() + i);
		++numRemoved;
    }

}

void Node::RemoveComponents(StringID type)
{
    unsigned numRemoved = 0;

    for (size_t i = components_.size() - 1; i < components_.size(); --i)
    {
        if (components_[i]->GetType() == type)
        {
            RemoveComponent(components_.begin() + i);
            ++numRemoved;
        }
    }

}

void Node::RemoveAllComponents()
{
    RemoveComponents();
}

void Node::ReorderComponent(Component* component, unsigned index)
{
    if (!component || component->GetNode() != this)
        return;

    for (Vector<SPtr<Component> >::iterator i = components_.begin(); i != components_.end(); ++i)
    {
        if (*i == component)
        {
            // Need shared ptr to insert. Also, prevent destruction when removing first
            SPtr<Component> componentShared(component);
            components_.erase(i);
            components_.insert(components_.begin() + index, componentShared);
            return;
        }
    }
}

Node* Node::Clone()
{
    // The scene itself can not be cloned
    if (this == scene_ || !parent_)
    {
        UNIQUE_LOGERROR("Can not clone node without a parent");
        return 0;
    }

    UNIQUE_PROFILE(CloneNode);

    SceneResolver resolver;
    Node* clone = CloneRecursive(parent_, resolver);
    resolver.Resolve();
    //clone->ApplyAttributes();
    return clone;
}

void Node::Remove()
{
    if (parent_)
        parent_->RemoveChild(this);
}

void Node::SetParent(Node* parent)
{
    if (parent)
    {
        Matrix3x4 oldWorldTransform = GetWorldTransform();

        parent->AddChild(this);

        if (parent != scene_)
        {
            Matrix3x4 newTransform = parent->GetWorldTransform().Inverse() * oldWorldTransform;
            SetTransform(newTransform.Translation(), newTransform.Rotation(), newTransform.Scale());
        }
        else
        {
            // The root node is assumed to have identity transform, so can disregard it
            SetTransform(oldWorldTransform.Translation(), oldWorldTransform.Rotation(), oldWorldTransform.Scale());
        }
    }
}

void Node::AddListener(Component* component)
{
    if (!component)
        return;

    // Check for not adding twice
    for (Vector<WPtr<Component> >::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
    {
        if (*i == component)
            return;
    }

    listeners_.push_back(WPtr<Component>(component));
    // If the node is currently dirty, notify immediately
    if (dirty_)
        component->OnMarkedDirty(this);
}

void Node::RemoveListener(Component* component)
{
    for (Vector<WPtr<Component> >::iterator i = listeners_.begin(); i != listeners_.end(); ++i)
    {
        if (*i == component)
        {
            listeners_.erase(i);
            return;
        }
    }
}

Vector3 Node::GetSignedWorldScale() const
{
    if (dirty_)
        UpdateWorldTransform();

    return worldTransform_.SignedScale(worldRotation_.RotationMatrix());
}

Vector3 Node::LocalToWorld(const Vector3& position) const
{
    return GetWorldTransform() * position;
}

Vector3 Node::LocalToWorld(const Vector4& vector) const
{
    return GetWorldTransform() * vector;
}

Vector2 Node::LocalToWorld2D(const Vector2& vector) const
{
    Vector3 result = LocalToWorld(Vector3(vector));
    return Vector2(result.x_, result.y_);
}

Vector3 Node::WorldToLocal(const Vector3& position) const
{
    return GetWorldTransform().Inverse() * position;
}

Vector3 Node::WorldToLocal(const Vector4& vector) const
{
    return GetWorldTransform().Inverse() * vector;
}

Vector2 Node::WorldToLocal2D(const Vector2& vector) const
{
    Vector3 result = WorldToLocal(Vector3(vector));
    return Vector2(result.x_, result.y_);
}

unsigned Node::GetNumChildren(bool recursive) const
{
    if (!recursive)
        return (unsigned)children_.size();
    else
    {
		uint allChildren = (uint)children_.size();
        for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
            allChildren += (*i)->GetNumChildren(true);

        return (unsigned)allChildren;
    }
}

void Node::GetChildren(PODVector<Node*>& dest, bool recursive) const
{
    dest.clear();

    if (!recursive)
    {
        for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
            dest.push_back(*i);
    }
    else
        GetChildrenRecursive(dest);
}

PODVector<Node*> Node::GetChildren(bool recursive) const
{
    PODVector<Node*> dest;
    GetChildren(dest, recursive);
    return dest;
}

void Node::GetChildrenWithComponent(PODVector<Node*>& dest, StringID type, bool recursive) const
{
    dest.clear();

    if (!recursive)
    {
        for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
        {
            if ((*i)->HasComponent(type))
                dest.push_back(*i);
        }
    }
    else
        GetChildrenWithComponentRecursive(dest, type);
}

PODVector<Node*> Node::GetChildrenWithComponent(StringID type, bool recursive) const
{
    PODVector<Node*> dest;
    GetChildrenWithComponent(dest, type, recursive);
    return dest;
}

void Node::GetChildrenWithTag(PODVector<Node*>& dest, const String& tag, bool recursive /*= true*/) const
{
    dest.clear();

    if (!recursive)
    {
        for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
        {
            if ((*i)->HasTag(tag))
                dest.push_back(*i);
        }
    }
    else
        GetChildrenWithTagRecursive(dest, tag);
}

PODVector<Node*> Node::GetChildrenWithTag(const String& tag, bool recursive) const
{
    PODVector<Node*> dest;
    GetChildrenWithTag(dest, tag, recursive);
    return dest;
}

Node* Node::GetChild(unsigned index) const
{
    return index < children_.size() ? children_[index].Get() : 0;
}

Node* Node::GetChild(const String& name, bool recursive) const
{
    return GetChild(StringID(name), recursive);
}

Node* Node::GetChild(const char* name, bool recursive) const
{
    return GetChild(StringID(name), recursive);
}

Node* Node::GetChild(StringID nameHash, bool recursive) const
{
    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
    {
        if ((*i)->GetName() == nameHash)
            return *i;

        if (recursive)
        {
            Node* node = (*i)->GetChild(nameHash, true);
            if (node)
                return node;
        }
    }

    return 0;
}

void Node::GetComponents(PODVector<Component*>& dest, StringID type, bool recursive) const
{
    dest.clear();

    if (!recursive)
    {
        for (Vector<SPtr<Component> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
        {
            if ((*i)->GetType() == type)
                dest.push_back(*i);
        }
    }
    else
        GetComponentsRecursive(dest, type);
}

bool Node::HasComponent(StringID type) const
{
    for (Vector<SPtr<Component> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
    {
        if ((*i)->GetType() == type)
            return true;
    }
    return false;
}

bool Node::HasTag(const String& tag) const
{
    return Contains(tags_, tag);
}

bool Node::IsChildOf(Node* node) const
{
    Node* parent = parent_;
    while (parent)
    {
        if (parent == node)
            return true;
        parent = parent->parent_;
    }
    return false;
}

Component* Node::GetComponent(StringID type, bool recursive) const
{
    for (Vector<SPtr<Component> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
    {
        if ((*i)->GetType() == type)
            return *i;
    }

    if (recursive)
    {
        for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
        {
            Component* component = (*i)->GetComponent(type, true);
            if (component)
                return component;
        }
    }

    return 0;
}

Component* Node::GetParentComponent(StringID type, bool fullTraversal) const
{
    Node* current = GetParent();
    while (current)
    {
        Component* soughtComponent = current->GetComponent(type);
        if (soughtComponent)
            return soughtComponent;

        if (fullTraversal)
            current = current->GetParent();
        else
            break;
    }
    return 0;
}

void Node::SetID(unsigned id)
{
    id_ = id;
}

void Node::SetScene(Scene* scene)
{
    scene_ = scene;
}

void Node::ResetScene()
{
    SetID(0);
    SetScene(0);
 //   SetOwner(0);
}

Node* Node::CreateChild(unsigned id, bool temporary)
{
    SPtr<Node> newNode(new Node());
	newNode->SetTemporary(temporary);

    // If zero ID specified, or the ID is already taken, let the scene assign
    if (scene_)
    {
        if (!id || scene_->GetNode(id))
            id = scene_->GetFreeNodeID();
        newNode->SetID(id);
    }
    else
        newNode->SetID(id);

    AddChild(newNode);
    return newNode;
}

void Node::AddComponent(Component* component, unsigned id)
{
    if (!component)
        return;

    components_.push_back(SPtr<Component>(component));

    if (component->GetNode())
        UNIQUE_LOGWARNING("Component " + component->GetType().ToString() + " already belongs to a node!");

    component->SetNode(this);

    // If zero ID specified, or the ID is already taken, let the scene assign
    if (scene_)
    {
        if (!id || scene_->GetComponent(id))
            id = scene_->GetFreeComponentID();
        component->SetID(id);
        scene_->ComponentAdded(component);
    }
    else
        component->SetID(id);

    component->OnMarkedDirty(this);

    // Send change event
    if (scene_)
    {
		ComponentAdded eventData;
		eventData.scene_ = scene_;
		eventData.node_ = this;
		eventData.component_ = component;
        scene_->SendEvent(eventData);
    }
}

unsigned Node::GetNumPersistentChildren() const
{
    unsigned ret = 0;

    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
    {
		if (!(*i)->IsTemporary())
            ++ret;
    }

    return ret;
}

unsigned Node::GetNumPersistentComponents() const
{
    unsigned ret = 0;

    for (Vector<SPtr<Component> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
    {
		if (!(*i)->IsTemporary())
            ++ret;
    }

    return ret;
}

void Node::SetTransformSilent(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
    position_ = position;
    rotation_ = rotation;
    scale_ = scale;
}

void Node::SetEnabledImpl(bool enable, bool recursive, bool storeSelf)
{
    // The enabled state of the whole scene can not be changed. SetUpdateEnabled() is used instead to start/stop updates.
    if (GetType() == Scene::GetTypeStatic())
    {
        UNIQUE_LOGERROR("Can not change enabled state of the Scene");
        return;
    }

    if (storeSelf)
        enabledPrev_ = enable;

    if (enable != enabled_)
    {
        enabled_ = enable;

        // Notify listener components of the state change
        for (Vector<WPtr<Component> >::iterator i = listeners_.begin(); i != listeners_.end();)
        {
            if (*i)
            {
                (*i)->OnNodeSetEnabled(this);
                ++i;
            }
            // If listener has expired, erase from list
            else
                i = listeners_.erase(i);
        }

        // Send change event
        if (scene_)
        {
			NodeEnabledChanged eventData;
			eventData.scene_ = scene_;
			eventData.node_ = this;
            scene_->SendEvent(eventData);
        }

        for (Vector<SPtr<Component> >::iterator i = components_.begin(); i != components_.end(); ++i)
        {
            (*i)->OnSetEnabled();

            // Send change event for the component
            if (scene_)
            {
				ComponentEnabledChanged eventData;
				eventData.scene_ = scene_;
				eventData.node_ = this;
				eventData.component_ = (*i).Get();
                scene_->SendEvent(eventData);
            }
        }
    }

    if (recursive)
    {
        for (Vector<SPtr<Node> >::iterator i = children_.begin(); i != children_.end(); ++i)
            (*i)->SetEnabledImpl(enable, recursive, storeSelf);
    }
}

void Node::UpdateWorldTransform() const
{
    Matrix3x4 transform = GetTransform();

    // Assume the root node (scene) has identity transform
    if (parent_ == scene_ || !parent_)
    {
        worldTransform_ = transform;
        worldRotation_ = rotation_;
    }
    else
    {
        worldTransform_ = parent_->GetWorldTransform() * transform;
        worldRotation_ = parent_->GetWorldRotation() * rotation_;
    }

    dirty_ = false;
}

void Node::RemoveChild(Vector<SPtr<Node> >::iterator i)
{
    // Keep a shared pointer to the child about to be removed, to make sure the erase from container completes first. Otherwise
    // it would be possible that other child nodes get removed as part of the node's components' cleanup, causing a re-entrant
    // erase and a crash
    SPtr<Node> child(*i);

    // Send change event. Do not send when this node is already being destroyed
    if (Refs() > 0 && scene_)
    {
		NodeRemoved eventData;
		eventData.scene_ = scene_;
		eventData.parent_ = this;
		eventData.node_ = child.Get();
        scene_->SendEvent(eventData);
    }

    child->parent_ = 0;
    child->MarkDirty();
    if (scene_)
        scene_->NodeRemoved(child);

    children_.erase(i);
}

void Node::GetChildrenRecursive(PODVector<Node*>& dest) const
{
    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
    {
        Node* node = *i;
        dest.push_back(node);
        if (!node->children_.empty())
            node->GetChildrenRecursive(dest);
    }
}

void Node::GetChildrenWithComponentRecursive(PODVector<Node*>& dest, StringID type) const
{
    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
    {
        Node* node = *i;
        if (node->HasComponent(type))
            dest.push_back(node);
        if (!node->children_.empty())
            node->GetChildrenWithComponentRecursive(dest, type);
    }
}

void Node::GetComponentsRecursive(PODVector<Component*>& dest, StringID type) const
{
    for (Vector<SPtr<Component> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
    {
        if ((*i)->GetType() == type)
            dest.push_back(*i);
    }
    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
        (*i)->GetComponentsRecursive(dest, type);
}

void Node::GetChildrenWithTagRecursive(PODVector<Node*>& dest, const String& tag) const
{
    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
    {
        Node* node = *i;
        if (node->HasTag(tag))
            dest.push_back(node);
        if (!node->children_.empty())
            node->GetChildrenWithTagRecursive(dest, tag);
    }
}

Node* Node::CloneRecursive(Node* parent, SceneResolver& resolver)
{/*
    // Create clone node
    Node* cloneNode = parent->CreateChild(0, (mode == REPLICATED && id_ < FIRST_LOCAL_ID) ? REPLICATED : LOCAL);
    resolver.AddNode(id_, cloneNode);

    // Copy attributes
    const Vector<AttributeInfo>* attributes = GetAttributes();
    for (unsigned j = 0; j < attributes->size(); ++j)
    {
        const AttributeInfo& attr = attributes->at(j);
        // Do not copy network-only attributes, as they may have unintended side effects
        if (attr.mode_ & TF_DEFAULT)
        {
            Variant value;
            OnGetAttribute(attr, value);
            cloneNode->OnSetAttribute(attr, value);
        }
    }

    // Clone components
    for (Vector<SPtr<Component> >::const_iterator i = components_.begin(); i != components_.end(); ++i)
    {
        Component* component = *i;
//         if (component->IsTemporary())
//             continue;

        Component* cloneComponent = cloneNode->CloneComponent(component,
            (mode == REPLICATED && component->GetID() < FIRST_LOCAL_ID) ? REPLICATED : LOCAL, 0);
        if (cloneComponent)
            resolver.AddComponent(component->GetID(), cloneComponent);
    }

    // Clone child nodes recursively
    for (Vector<SPtr<Node> >::const_iterator i = children_.begin(); i != children_.end(); ++i)
    {
        Node* node = *i;
//         if (node->IsTemporary())
//             continue;

        node->CloneRecursive(cloneNode, resolver, mode);
    }

    {
        using namespace NodeCloned;

        VariantMap& eventData = GetEventDataMap();
        eventData[P_SCENE] = scene_;
        eventData[P_NODE] = this;
        eventData[P_CLONENODE] = cloneNode;

        scene_->SendEvent(E_NODECLONED, eventData);
    }

    return cloneNode;*/
	return nullptr;
}

void Node::RemoveComponent(Vector<SPtr<Component> >::iterator i)
{
    // Send node change event. Do not send when already being destroyed
    if (Refs() > 0 && scene_)
    {
		ComponentRemoved eventData;
		eventData.scene_ = scene_;
		eventData.node_ = this;
		eventData.component_ = (*i).Get();
        scene_->SendEvent(eventData);
    }

    RemoveListener(*i);
    if (scene_)
        scene_->ComponentRemoved(*i);
    (*i)->SetNode(0);
    components_.erase(i);
}


}
