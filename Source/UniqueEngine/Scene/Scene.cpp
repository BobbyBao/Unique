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
#include "Core/CoreEvents.h"
#include "Core/Profiler.h"
#include "Core/WorkQueue.h"
#include "IO/File.h"
#include "IO/Log.h"
#include "IO/PackageFile.h"
#include "Resource/ResourceCache.h"
#include "Resource/ResourceEvents.h"
#include "../Scene/Component.h"
//#include "../Scene/ObjectAnimation.h"
#include "../Scene/Scene.h"
#include "../Scene/SceneEvents.h"
#include "../Scene/SmoothedTransform.h"
//#include "../Scene/SplinePath.h"
//#include "../Scene/UnknownComponent.h"
//#include "../Scene/ValueAnimation.h"

//#include "../DebugNew.h"

namespace Unique
{

const char* SCENE_CATEGORY = "Scene";
const char* LOGIC_CATEGORY = "Logic";
const char* SUBSYSTEM_CATEGORY = "Subsystem";

static const float DEFAULT_SMOOTHING_CONSTANT = 50.0f;
static const float DEFAULT_SNAP_THRESHOLD = 5.0f;

uObject(Scene)
{
}

Scene::Scene() :
    localNodeID_(FIRST_LOCAL_ID),
    localComponentID_(FIRST_LOCAL_ID),
    checksum_(0),
    asyncLoadingMs_(5),
    timeScale_(1.0f),
    elapsedTime_(0),
    smoothingConstant_(DEFAULT_SMOOTHING_CONSTANT),
    snapThreshold_(DEFAULT_SNAP_THRESHOLD),
    updateEnabled_(true),
    asyncLoading_(false),
    threadedUpdate_(false)
{
    // Assign an ID to self so that nodes can refer to this node as a parent
    SetID(GetFreeNodeID());
    NodeAdded(this);

    SubscribeToEvent(&Scene::HandleUpdate);
    SubscribeToEvent(&Scene::HandleResourceBackgroundLoaded);
}

Scene::~Scene()
{
    // Remove root-level components first, so that scene subsystems such as the octree destroy themselves. This will speed up
    // the removal of child nodes' components
    RemoveAllComponents();
    RemoveAllChildren();

    for (HashMap<unsigned, Node*>::iterator i = localNodes_.begin(); i != localNodes_.end(); ++i)
        i->second->ResetScene();
}
/*
void Scene::RegisterObject()
{
    context->RegisterFactory<Scene>();

//    UNIQUE_ACCESSOR_ATTRIBUTE("Name", GetName, SetName, String, String::EMPTY, TF_DEFAULT);
    UNIQUE_ACCESSOR_ATTRIBUTE("Time Scale", GetTimeScale, SetTimeScale, float, 1.0f, TF_DEFAULT);
    UNIQUE_ACCESSOR_ATTRIBUTE("Smoothing Constant", GetSmoothingConstant, SetSmoothingConstant, float, DEFAULT_SMOOTHING_CONSTANT,
        TF_DEFAULT);
    UNIQUE_ACCESSOR_ATTRIBUTE("Snap Threshold", GetSnapThreshold, SetSnapThreshold, float, DEFAULT_SNAP_THRESHOLD, TF_DEFAULT);
    UNIQUE_ACCESSOR_ATTRIBUTE("Elapsed Time", GetElapsedTime, SetElapsedTime, float, 0.0f, TF_DEFAULT);
    UNIQUE_ATTRIBUTE("Next Replicated Node ID", unsigned, replicatedNodeID_, FIRST_REPLICATED_ID, TF_DEFAULT | AM_NOEDIT);
    UNIQUE_ATTRIBUTE("Next Replicated Component ID", unsigned, replicatedComponentID_, FIRST_REPLICATED_ID, TF_DEFAULT | AM_NOEDIT);
    UNIQUE_ATTRIBUTE("Next Local Node ID", unsigned, localNodeID_, FIRST_LOCAL_ID, TF_DEFAULT | AM_NOEDIT);
    UNIQUE_ATTRIBUTE("Next Local Component ID", unsigned, localComponentID_, FIRST_LOCAL_ID, TF_DEFAULT | AM_NOEDIT);
    UNIQUE_ATTRIBUTE("Variables", VariantMap, vars_, Variant::emptyVariantMap, TF_DEFAULT); // Network replication of vars uses custom data
    UNIQUE_MIXED_ACCESSOR_ATTRIBUTE("Variable Names", GetVarNamesAttr, SetVarNamesAttr, String, String::EMPTY, TF_DEFAULT | AM_NOEDIT);
}
*/
void Scene::Clear()
{
//    StopAsyncLoading();

    RemoveChildren(true);
    RemoveComponents();

	UnregisterAllVars();
	SetName(String::EMPTY);
	fileName_.Clear();
	checksum_ = 0;

	localNodeID_ = FIRST_LOCAL_ID;
	localComponentID_ = FIRST_LOCAL_ID;
}

void Scene::SetUpdateEnabled(bool enable)
{
    updateEnabled_ = enable;
}

void Scene::SetTimeScale(float scale)
{
    timeScale_ = Max(scale, M_EPSILON);
}

void Scene::SetSmoothingConstant(float constant)
{
    smoothingConstant_ = Max(constant, M_EPSILON);
}

void Scene::SetSnapThreshold(float threshold)
{
    snapThreshold_ = Max(threshold, 0.0f);
}

void Scene::SetAsyncLoadingMs(int ms)
{
    asyncLoadingMs_ = Max(ms, 1);
}

void Scene::SetElapsedTime(float time)
{
    elapsedTime_ = time;
}

void Scene::RegisterVar(const String& name)
{
    varNames_[name] = name;
}

void Scene::UnregisterVar(const String& name)
{
    varNames_.erase(name);
}

void Scene::UnregisterAllVars()
{
    varNames_.clear();
}

Node* Scene::GetNode(unsigned id) const
{
        HashMap<unsigned, Node*>::const_iterator i = localNodes_.find(id);
		return i != localNodes_.end() ? i->second : 0;
}

bool Scene::GetNodesWithTag(PODVector<Node*>& dest, const String& tag) const
{
    dest.clear();
    HashMap<StringID, PODVector<Node*> >::const_iterator it = taggedNodes_.find(tag);
    if (it != taggedNodes_.end())
    {
        dest = it->second;
        return true;
    }
    else
        return false;
}

Component* Scene::GetComponent(unsigned id) const
{
	HashMap<unsigned, Component*>::const_iterator i = localComponents_.find(id);
	return i != localComponents_.end() ? i->second : 0;
   
}

// float Scene::GetAsyncProgress() const
// {
//     return !asyncLoading_ || asyncProgress_.totalNodes_ + asyncProgress_.totalResources_ == 0 ? 1.0f :
//         (float)(asyncProgress_.loadedNodes_ + asyncProgress_.loadedResources_) /
//         (float)(asyncProgress_.totalNodes_ + asyncProgress_.totalResources_);
// }

const String& Scene::GetVarName(StringID hash) const
{
    HashMap<StringID, String>::const_iterator i = varNames_.find(hash);
    return i != varNames_.end() ? i->second : String::EMPTY;
}

void Scene::Update(float timeStep)
{
#if false
    if (asyncLoading_)
    {
        UpdateAsyncLoading();
        // If only preloading resources, scene update can continue
        if (asyncProgress_.mode_ > LOAD_RESOURCES_ONLY)
            return;
    }
#endif
    UNIQUE_PROFILE(UpdateScene);

    timeStep *= timeScale_;

	SceneUpdate eventData;
	eventData.scene_ = this;
	eventData.timeStep_ = timeStep;

    // Update variable timestep logic
    SendEvent(eventData);

    // Update scene attribute animation.
    SendEvent((const AttributeAnimationUpdate&)eventData);

    // Update scene subsystems. If a physics world is present, it will be updated, triggering fixed timestep logic updates
    SendEvent((const SceneSubsystemUpdate&)eventData);

    // Update transform smoothing
    {
        UNIQUE_PROFILE(UpdateSmoothing);

        float constant = 1.0f - Clamp(powf(2.0f, -timeStep * smoothingConstant_), 0.0f, 1.0f);
        float squaredSnapThreshold = snapThreshold_ * snapThreshold_;

		UpdateSmoothing eventData;
		eventData.constant_ = constant;
		eventData.squaredSnapThreshold_ = squaredSnapThreshold;
        SendEvent(eventData);
    }

    // Post-update variable timestep logic
    SendEvent((const ScenePostUpdate&)eventData);

    // Note: using a float for elapsed time accumulation is inherently inaccurate. The purpose of this value is
    // primarily to update material animation effects, as it is available to shaders. It can be reset by calling
    // SetElapsedTime()
    elapsedTime_ += timeStep;
}

void Scene::BeginThreadedUpdate()
{
    // Check the work queue subsystem whether it actually has created worker threads. If not, do not enter threaded mode.
    if (Subsystem<WorkQueue>().GetNumThreads())
        threadedUpdate_ = true;
}

void Scene::EndThreadedUpdate()
{
    if (!threadedUpdate_)
        return;

    threadedUpdate_ = false;

    if (!delayedDirtyComponents_.empty())
    {
        UNIQUE_PROFILE(EndThreadedUpdate);

        for (PODVector<Component*>::const_iterator i = delayedDirtyComponents_.begin(); i != delayedDirtyComponents_.end(); ++i)
            (*i)->OnMarkedDirty((*i)->GetNode());
        delayedDirtyComponents_.clear();
    }
}

void Scene::DelayedMarkedDirty(Component* component)
{
    std::lock_guard<std::mutex> lock(sceneMutex_);
    delayedDirtyComponents_.push_back(component);
}

unsigned Scene::GetFreeNodeID()
{
    for (;;)
    {
        unsigned ret = localNodeID_;
        if (localNodeID_ < LAST_LOCAL_ID)
            ++localNodeID_;
        else
            localNodeID_ = FIRST_LOCAL_ID;

        if (!localNodes_.count(ret))
            return ret;
    }
    
}

unsigned Scene::GetFreeComponentID()
{
	for (;;)
	{
		unsigned ret = localComponentID_;
		if (localComponentID_ < LAST_LOCAL_ID)
			++localComponentID_;
		else
			localComponentID_ = FIRST_LOCAL_ID;

		if (!localComponents_.count(ret))
			return ret;
	}

}

void Scene::NodeAdded(Node* node)
{
    if (!node || node->GetScene() == this)
        return;

    // Remove from old scene first
    Scene* oldScene = node->GetScene();
    if (oldScene)
        oldScene->NodeRemoved(node);

    node->SetScene(this);

    // If the new node has an ID of zero (default), assign a replicated ID now
    unsigned id = node->GetID();
    if (!id)
    {
        id = GetFreeNodeID();
        node->SetID(id);
    }

	HashMap<unsigned, Node*>::iterator i = localNodes_.find(id);
	if (i != localNodes_.end() && i->second != node)
	{
		UNIQUE_LOGWARNING("Overwriting node with ID " + String(id));
		NodeRemoved(i->second);
	}
	localNodes_[id] = node;
  
    // Cache tag if already tagged.
    if (!node->GetTags().empty())
    {
        const StringVector& tags = node->GetTags();
        for (unsigned i = 0; i < tags.size(); ++i)
            taggedNodes_[tags[i]].push_back(node);
    }

    // Add already created components and child nodes now
    const Vector<SPtr<Component> >& components = node->GetComponents();
    for (Vector<SPtr<Component> >::const_iterator i = components.begin(); i != components.end(); ++i)
        ComponentAdded(*i);
    const Vector<SPtr<Node> >& children = node->GetChildren();
    for (Vector<SPtr<Node> >::const_iterator i = children.begin(); i != children.end(); ++i)
        NodeAdded(*i);
}

void Scene::NodeTagAdded(Node* node, const String& tag)
{
    taggedNodes_[tag].push_back(node);
}

void Scene::NodeTagRemoved(Node* node, const String& tag)
{
    Unique::Remove(taggedNodes_[tag], node);
}

void Scene::NodeRemoved(Node* node)
{
    if (!node || node->GetScene() != this)
        return;

    unsigned id = node->GetID();

	localNodes_.erase(id);

    node->ResetScene();

    // Remove node from tag cache
    if (!node->GetTags().empty())
    {
        const StringVector& tags = node->GetTags();
        for (unsigned i = 0; i < tags.size(); ++i)
            Unique::Remove(taggedNodes_[tags[i]], node);
    }

    // Remove components and child nodes as well
    const Vector<SPtr<Component> >& components = node->GetComponents();
    for (Vector<SPtr<Component> >::const_iterator i = components.begin(); i != components.end(); ++i)
        ComponentRemoved(*i);
    const Vector<SPtr<Node> >& children = node->GetChildren();
    for (Vector<SPtr<Node> >::const_iterator i = children.begin(); i != children.end(); ++i)
        NodeRemoved(*i);
}

void Scene::ComponentAdded(Component* component)
{
    if (!component)
        return;

    unsigned id = component->GetID();

    // If the new component has an ID of zero (default), assign a replicated ID now
    if (!id)
    {
        id = GetFreeComponentID();
        component->SetID(id);
    }

	HashMap<unsigned, Component*>::iterator i = localComponents_.find(id);
	if (i != localComponents_.end() && i->second != component)
	{
		UNIQUE_LOGWARNING("Overwriting component with ID " + String(id));
		ComponentRemoved(i->second);
	}

	localComponents_[id] = component;

    component->OnSceneSet(this);
}

void Scene::ComponentRemoved(Component* component)
{
    if (!component)
        return;

	unsigned id = component->GetID();
    localComponents_.erase(id);

    component->SetID(0);
    component->OnSceneSet(0);
}

void Scene::SetVarNamesAttr(const String& value)
{
    Vector<String> varNames = value.Split(';');

    varNames_.clear();
    for (Vector<String>::const_iterator i = varNames.begin(); i != varNames.end(); ++i)
        varNames_[*i] = *i;
}

String Scene::GetVarNamesAttr() const
{
    String ret;

    if (!varNames_.empty())
    {
        for (HashMap<StringID, String>::const_iterator i = varNames_.begin(); i != varNames_.end(); ++i)
            ret += i->second + ";";

        ret.Resize(ret.Length() - 1);
    }

    return ret;
}

void Scene::HandleUpdate(StringID eventType, const Unique::Update& eventData)
{
    if (!updateEnabled_)
        return;

    Update(eventData.timeStep_);
}

void Scene::HandleResourceBackgroundLoaded(StringID eventType, const ResourceBackgroundLoaded& eventData)
{
    if (asyncLoading_)
    {
        Resource* resource = static_cast<Resource*>(eventData.resource_);
//         if (asyncProgress_.resources_.Contains(resource->GetNameHash()))
//         {
//             asyncProgress_.resources_.erase(resource->GetNameHash());
//             ++asyncProgress_.loadedResources_;
//         }
    }
}

#if false
void Scene::UpdateAsyncLoading()
{
    UNIQUE_PROFILE(UpdateAsyncLoading);

    // If resources left to load, do not load nodes yet
    if (asyncProgress_.loadedResources_ < asyncProgress_.totalResources_)
        return;

    HiresTimer asyncLoadTimer;

    for (;;)
    {
        if (asyncProgress_.loadedNodes_ >= asyncProgress_.totalNodes_)
        {
            FinishAsyncLoading();
            return;
        }


        // Read one child node with its full sub-hierarchy either from binary, JSON, or XML
        /// \todo Works poorly in scenes where one root-level child node contains all content
        if (asyncProgress_.xmlFile_)
        {
            unsigned nodeID = asyncProgress_.xmlElement_.GetUInt("id");
            Node* newNode = CreateChild(nodeID, nodeID < FIRST_LOCAL_ID ? REPLICATED : LOCAL);
            resolver_.AddNode(nodeID, newNode);
            newNode->LoadXML(asyncProgress_.xmlElement_, resolver_);
            asyncProgress_.xmlElement_ = asyncProgress_.xmlElement_.GetNext("node");
        }
        else if (asyncProgress_.jsonFile_) // Load from JSON
        {
            const JSONValue& childValue = asyncProgress_.jsonFile_->GetRoot().Get("children").GetArray().At(asyncProgress_.jsonIndex_);

            unsigned nodeID =childValue.Get("id").GetUInt();
            Node* newNode = CreateChild(nodeID, nodeID < FIRST_LOCAL_ID ? REPLICATED : LOCAL);
            resolver_.AddNode(nodeID, newNode);
            newNode->LoadJSON(childValue, resolver_);
            ++asyncProgress_.jsonIndex_;
        }
        else // Load from binary
        {
            unsigned nodeID = asyncProgress_.file_->ReadUInt();
            Node* newNode = CreateChild(nodeID, nodeID < FIRST_LOCAL_ID ? REPLICATED : LOCAL);
            resolver_.AddNode(nodeID, newNode);
            newNode->Load(*asyncProgress_.file_, resolver_);
        }

        ++asyncProgress_.loadedNodes_;

        // Break if time limit exceeded, so that we keep sufficient FPS
        if (asyncLoadTimer.GetUSec(false) >= asyncLoadingMs_ * 1000)
            break;
    }

    using namespace AsyncLoadProgress;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_SCENE] = this;
    eventData[P_PROGRESS] = GetAsyncProgress();
    eventData[P_LOADEDNODES] = asyncProgress_.loadedNodes_;
    eventData[P_TOTALNODES] = asyncProgress_.totalNodes_;
    eventData[P_LOADEDRESOURCES] = asyncProgress_.loadedResources_;
    eventData[P_TOTALRESOURCES] = asyncProgress_.totalResources_;
    SendEvent(E_ASYNCLOADPROGRESS, eventData);

}

void Scene::FinishAsyncLoading()
{
    if (asyncProgress_.mode_ > LOAD_RESOURCES_ONLY)
    {
        resolver_.Resolve();
        ApplyAttributes();
        FinishLoading(asyncProgress_.file_);
    }

//    StopAsyncLoading();

    using namespace AsyncLoadFinished;

    VariantMap& eventData = GetEventDataMap();
    eventData[P_SCENE] = this;
    SendEvent(E_ASYNCLOADFINISHED, eventData);
}

void Scene::FinishLoading(Deserializer* source)
{
    if (source)
    {
        fileName_ = source->GetName();
        checksum_ = source->GetChecksum();
    }
}

void Scene::FinishSaving(Serializer* dest) const
{
    Deserializer* ptr = dynamic_cast<Deserializer*>(dest);
    if (ptr)
    {
        fileName_ = ptr->GetName();
        checksum_ = ptr->GetChecksum();
    }
}

#endif
}
