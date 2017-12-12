#pragma once

//#include "../Core/Mutex.h"
#include "../Scene/Node.h"
#include "../Scene/SceneResolver.h"

namespace Unique
{
struct Update;
class File;
class PackageFile;

static const unsigned FIRST_REPLICATED_ID = 0x1;
static const unsigned LAST_REPLICATED_ID = 0xffffff;
static const unsigned FIRST_LOCAL_ID = 0x01000000;
static const unsigned LAST_LOCAL_ID = 0xffffffff;

/// Root scene node, represents the whole scene.
class UNIQUE_API Scene : public Node
{
	uRTTI(Scene, Node)

    using Node::GetComponent;

public:
    /// Construct.
    Scene();
    /// Destruct.
    virtual ~Scene();

	/// Clear scene completely of either replicated, local or all nodes and components.
    void Clear();
    /// Enable or disable scene update.
    void SetUpdateEnabled(bool enable);
    /// Set update time scale. 1.0 = real time (default.)
    void SetTimeScale(float scale);
    /// Set elapsed time in seconds. This can be used to prevent inaccuracy in the timer if the scene runs for a long time.
    void SetElapsedTime(float time);
    /// Set network client motion smoothing constant.
    void SetSmoothingConstant(float constant);
    /// Set network client motion smoothing snap threshold.
    void SetSnapThreshold(float threshold);
    /// Set maximum milliseconds per frame to spend on async scene loading.
    void SetAsyncLoadingMs(int ms);
    /// Add a required package file for networking. To be called on the server.
    void AddRequiredPackageFile(PackageFile* package);
    /// Clear required package files.
    void ClearRequiredPackageFiles();
    /// Register a node user variable hash reverse mapping (for editing.)
    void RegisterVar(const String& name);
    /// Unregister a node user variable hash reverse mapping.
    void UnregisterVar(const String& name);
    /// Clear all registered node user variable hash reverse mappings.
    void UnregisterAllVars();

    /// Return node from the whole scene by ID, or null if not found.
    Node* GetNode(unsigned id) const;
    /// Return component from the whole scene by ID, or null if not found.
    Component* GetComponent(unsigned id) const;
    /// Get nodes with specific tag from the whole scene, return false if empty.
    bool GetNodesWithTag(PODVector<Node*>& dest, const String& tag)  const;

    /// Return whether updates are enabled.
    bool IsUpdateEnabled() const { return updateEnabled_; }

    /// Return whether an asynchronous loading operation is in progress.
    bool IsAsyncLoading() const { return asyncLoading_; }

    /// Return asynchronous loading progress between 0.0 and 1.0, or 1.0 if not in progress.
    float GetAsyncProgress() const;

    /// Return source file name.
    const String& GetFileName() const { return fileName_; }

    /// Return source file checksum.
    unsigned GetChecksum() const { return checksum_; }

    /// Return update time scale.
    float GetTimeScale() const { return timeScale_; }

    /// Return elapsed time in seconds.
    float GetElapsedTime() const { return elapsedTime_; }

    /// Return motion smoothing constant.
    float GetSmoothingConstant() const { return smoothingConstant_; }

    /// Return motion smoothing snap threshold.
    float GetSnapThreshold() const { return snapThreshold_; }

    /// Return maximum milliseconds per frame to spend on async loading.
    int GetAsyncLoadingMs() const { return asyncLoadingMs_; }

    /// Return a node user variable name, or empty if not registered.
    const String& GetVarName(StringID hash) const;

    /// Update scene. Called by HandleUpdate.
    void Update(float timeStep);
    /// Begin a threaded update. During threaded update components can choose to delay dirty processing.
    void BeginThreadedUpdate();
    /// End a threaded update. Notify components that marked themselves for delayed dirty processing.
    void EndThreadedUpdate();
    /// Add a component to the delayed dirty notify queue. Is thread-safe.
    void DelayedMarkedDirty(Component* component);

    /// Return threaded update flag.
    bool IsThreadedUpdate() const { return threadedUpdate_; }

    /// Get free node ID, either non-local or local.
    unsigned GetFreeNodeID();
    /// Get free component ID, either non-local or local.
    unsigned GetFreeComponentID();

    /// Cache node by tag if tag not zero, no checking if already added. Used internaly in Node::AddTag.
    void NodeTagAdded(Node* node, const String& tag);
    /// Cache node by tag if tag not zero.
    void NodeTagRemoved(Node* node, const String& tag);

    /// Node added. Assign scene pointer and add to ID map.
    void NodeAdded(Node* node);
    /// Node removed. Remove from ID map.
    void NodeRemoved(Node* node);
    /// Component added. Add to ID map.
    void ComponentAdded(Component* component);
    /// Component removed. Remove from ID map.
    void ComponentRemoved(Component* component);
    /// Set node user variable reverse mappings.
    void SetVarNamesAttr(const String& value);
    /// Return node user variable reverse mappings.
    String GetVarNamesAttr() const;

private:
    /// Handle the logic update event to update the scene, if active.
    void HandleUpdate(const Unique::Update& eventData);
    /// Handle a background loaded resource completing.
    void HandleResourceBackgroundLoaded(const struct ResourceBackgroundLoaded& eventData);
#if false
    /// Update asynchronous loading.
    void UpdateAsyncLoading();
    /// Finish asynchronous loading.
    void FinishAsyncLoading();
    /// Finish loading. Sets the scene filename and checksum.
    void FinishLoading(Deserializer* source);
    /// Finish saving. Sets the scene filename and checksum.
    void FinishSaving(Serializer* dest) const;

    /// Preload resources from a binary scene or object prefab file.
    void PreloadResources(File* file, bool isSceneFile);
    /// Preload resources from an XML scene or object prefab file.
    void PreloadResourcesXML(const XMLElement& element);
    /// Preload resources from a JSON scene or object prefab file.
    void PreloadResourcesJSON(const JSONValue& value);
#endif

    /// Local scene nodes by ID.
    HashMap<unsigned, Node*> localNodes_;
    /// Local components by ID.
    HashMap<unsigned, Component*> localComponents_;
    /// Cached tagged nodes by tag.
    HashMap<StringID, PODVector<Node*> > taggedNodes_;
    /// Node and component ID resolver for asynchronous loading.
    SceneResolver resolver_;
    /// Source file name.
    mutable String fileName_;
    /// Registered node user variable reverse mappings.
    HashMap<StringID, String> varNames_;
    /// Delayed dirty notification queue for components.
    PODVector<Component*> delayedDirtyComponents_;
    /// Mutex for the delayed dirty notification queue.
    Mutex sceneMutex_;
    /// Next free local node ID.
    unsigned localNodeID_;
    /// Next free local component ID.
    unsigned localComponentID_;
    /// Scene source file checksum.
    mutable unsigned checksum_;
    /// Maximum milliseconds per frame to spend on async scene loading.
    int asyncLoadingMs_;
    /// Scene update time scale.
    float timeScale_;
    /// Elapsed time accumulator.
    float elapsedTime_;
    /// Motion smoothing constant.
    float smoothingConstant_;
    /// Motion smoothing snap threshold.
    float snapThreshold_;
    /// Update enabled flag.
    bool updateEnabled_;
    /// Asynchronous loading flag.
    bool asyncLoading_;
    /// Threaded update flag.
    bool threadedUpdate_;
};


}
