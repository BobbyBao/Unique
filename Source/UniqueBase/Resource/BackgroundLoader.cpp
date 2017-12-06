
#include "Precompiled.h"
#include "Unique.h"

#include "../Core/Context.h"
#include "../Core/Profiler.h"
#include "../IO/Log.h"
#include "../Resource/BackgroundLoader.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/ResourceEvents.h"

#include "../DebugNew.h"

namespace Unique
{

BackgroundLoader::BackgroundLoader(ResourceCache* owner) :
    owner_(owner)
{
}

BackgroundLoader::~BackgroundLoader()
{
    std::lock_guard<Mutex> lock(backgroundLoadMutex_);

    backgroundLoadQueue_.clear();
}

void BackgroundLoader::ThreadFunction()
{
    while (shouldRun_)
    {
        backgroundLoadMutex_.lock();

        // Search for a queued resource that has not been loaded yet
        HashMap<Pair<StringID, StringID>, BackgroundLoadItem>::iterator i = backgroundLoadQueue_.begin();
        while (i != backgroundLoadQueue_.end())
        {
            if (i->second.resource_->GetAsyncLoadState() == ASYNC_QUEUED)
                break;
            else
                ++i;
        }

        if (i == backgroundLoadQueue_.end())
        {
            // No resources to load found
            backgroundLoadMutex_.unlock();
            Time::Sleep(5);
        }
        else
        {
            BackgroundLoadItem& item = i->second;
            Resource* resource = item.resource_;
            // We can be sure that the item is not removed from the queue as long as it is in the
            // "queued" or "loading" state
            backgroundLoadMutex_.unlock();

            bool success = false;
            SPtr<File> file = owner_->GetFile(resource->GetName(), item.sendEventOnFailure_);
            if (file)
            {
                resource->SetAsyncLoadState(ASYNC_LOADING);
                success = resource->Prepare();
            }

            // Process dependencies now
            // Need to lock the queue again when manipulating other entries
            Pair<StringID, StringID> key = std::make_pair(resource->GetType(), resource->GetNameHash());
            backgroundLoadMutex_.lock();
            if (item.dependents_.size())
            {
                for (HashSet<Pair<StringID, StringID> >::iterator i = item.dependents_.begin();
                     i != item.dependents_.end(); ++i)
                {
                    HashMap<Pair<StringID, StringID>, BackgroundLoadItem>::iterator j = backgroundLoadQueue_.find(*i);
                    if (j != backgroundLoadQueue_.end())
                        j->second.dependencies_.erase(key);
                }

                item.dependents_.clear();
            }

            resource->SetAsyncLoadState(success ? ASYNC_SUCCESS : ASYNC_FAIL);
            backgroundLoadMutex_.unlock();
        }
    }
}

bool BackgroundLoader::QueueResource(StringID type, const String& name, bool sendEventOnFailure, Resource* caller)
{
    StringID nameHash(name);
    Pair<StringID, StringID> key = std::make_pair(type, nameHash);

    std::lock_guard<Mutex> lock(backgroundLoadMutex_);

    // Check if already exists in the queue
    if (backgroundLoadQueue_.find(key) != backgroundLoadQueue_.end())
        return false;

    BackgroundLoadItem& item = backgroundLoadQueue_[key];
    item.sendEventOnFailure_ = sendEventOnFailure;

    // Make sure the pointer is non-null and is a Resource subclass
    item.resource_ = DynamicCast<Resource>(GetContext()->CreateObject(type));
    if (!item.resource_)
    {
        UNIQUE_LOGERROR("Could not load unknown resource type " + String(type));

        if (sendEventOnFailure && Thread::IsMainThread())
        {
			UnknownResourceType eventData;
			eventData.resourceType_ = type;
            owner_->SendEvent(eventData);
        }

        backgroundLoadQueue_.erase(key);
        return false;
    }

    UNIQUE_LOGDEBUG("Background loading resource " + name);

    item.resource_->SetName(name);
    item.resource_->SetAsyncLoadState(ASYNC_QUEUED);

    // If this is a resource calling for the background load of more resources, mark the dependency as necessary
    if (caller)
    {
        Pair<StringID, StringID> callerKey = std::make_pair(caller->GetType(), caller->GetNameHash());
        HashMap<Pair<StringID, StringID>, BackgroundLoadItem>::iterator j = backgroundLoadQueue_.find(callerKey);
        if (j != backgroundLoadQueue_.end())
        {
            BackgroundLoadItem& callerItem = j->second;
            item.dependents_.insert(callerKey);
            callerItem.dependencies_.insert(key);
        }
        else
            UNIQUE_LOGWARNING("Resource " + caller->GetName() +
                       " requested for a background loaded resource but was not in the background load queue");
    }

    // Start the background loader thread now
    if (!IsStarted())
        Run();

    return true;
}

void BackgroundLoader::WaitForResource(StringID type, StringID nameHash)
{
    backgroundLoadMutex_.lock();

    // Check if the resource in question is being background loaded
    Pair<StringID, StringID> key = std::make_pair(type, nameHash);
    HashMap<Pair<StringID, StringID>, BackgroundLoadItem>::iterator i = backgroundLoadQueue_.find(key);
    if (i != backgroundLoadQueue_.end())
    {
        backgroundLoadMutex_.unlock();

        {
            Resource* resource = i->second.resource_;
            HiresTimer waitTimer;
            bool didWait = false;

            for (;;)
            {
                size_t numDeps = i->second.dependencies_.size();
                AsyncLoadState state = resource->GetAsyncLoadState();
                if (numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
                {
                    didWait = true;
                    Time::Sleep(1);
                }
                else
                    break;
            }

            if (didWait)
                UNIQUE_LOGDEBUG("Waited " + String(waitTimer.GetUSec(false) / 1000) + " ms for background loaded resource " +
                         resource->GetName());
        }

        // This may take a long time and may potentially wait on other resources, so it is important we do not hold the mutex during this
        FinishBackgroundLoading(i->second);

        backgroundLoadMutex_.lock();
        backgroundLoadQueue_.erase(i);
        backgroundLoadMutex_.unlock();
    }
    else
        backgroundLoadMutex_.unlock();
}

void BackgroundLoader::FinishResources(int maxMs)
{
    if (IsStarted())
    {
        HiresTimer timer;

        backgroundLoadMutex_.lock();

        for (HashMap<Pair<StringID, StringID>, BackgroundLoadItem>::iterator i = backgroundLoadQueue_.begin();
             i != backgroundLoadQueue_.end();)
        {
            Resource* resource = i->second.resource_;
            size_t numDeps = i->second.dependencies_.size();
            AsyncLoadState state = resource->GetAsyncLoadState();
            if (numDeps > 0 || state == ASYNC_QUEUED || state == ASYNC_LOADING)
                ++i;
            else
            {
                // Finishing a resource may need it to wait for other resources to load, in which case we can not
                // hold on to the mutex
                backgroundLoadMutex_.unlock();
                FinishBackgroundLoading(i->second);
                backgroundLoadMutex_.lock();
                i = backgroundLoadQueue_.erase(i);
            }

            // Break when the time limit passed so that we keep sufficient FPS
            if (timer.GetUSec(false) >= maxMs * 1000)
                break;
        }

        backgroundLoadMutex_.unlock();
    }
}

unsigned BackgroundLoader::GetNumQueuedResources() const
{
    std::lock_guard<Mutex> lock(backgroundLoadMutex_);
    return (unsigned)backgroundLoadQueue_.size();
}

void BackgroundLoader::FinishBackgroundLoading(BackgroundLoadItem& item)
{
    Resource* resource = item.resource_;

    bool success = resource->GetAsyncLoadState() == ASYNC_SUCCESS;
    // If BeginLoad() phase was successful, call EndLoad() and get the final success/failure result
    if (success)
    {
#ifdef UNIQUE_PROFILING
        String profileBlockName("Finish" + resource->GetTypeName());

        Profiler* profiler = owner_->Subsystem<Profiler>();
        if (profiler)
            profiler->BeginBlock(profileBlockName.CString());
#endif
        UNIQUE_LOGDEBUG("Finishing background loaded resource " + resource->GetName());
        success = resource->Create();

#ifdef UNIQUE_PROFILING
        if (profiler)
            profiler->EndBlock();
#endif
    }
    resource->SetAsyncLoadState(ASYNC_DONE);

    if (!success && item.sendEventOnFailure_)
    {
		LoadFailed eventData;
		eventData.resourceName_ = resource->GetName();
        owner_->SendEvent(eventData);
    }

    // Send event, either success or failure
    {
		ResourceBackgroundLoaded eventData;
		eventData.resourceName_ = resource->GetName();
		eventData.success_ = success;
		eventData.resource_ = resource;
        owner_->SendEvent(eventData);
    }

    // Store to the cache; use same mechanism as for manual resources
    if (success || owner_->GetReturnFailedResources())
        owner_->AddManualResource(resource);
}

}

