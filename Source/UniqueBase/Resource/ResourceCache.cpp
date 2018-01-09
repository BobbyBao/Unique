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
#include "../Core/CoreEvents.h"
#include "../Core/Profiler.h"
#include "../Core/WorkQueue.h"
#include "../IO/FileSystem.h"
#include "../IO/FileWatcher.h"
#include "../IO/Log.h"
#include "../IO/PackageFile.h"
#include "../Resource/BackgroundLoader.h"
//#include "../Resource/Image.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/ResourceEvents.h"
#include "../DebugNew.h"

#include <cstdio>

namespace Unique
{

static const char* checkDirs[] =
{
    "Fonts",
    "Materials",
    "Models",
    "Music",
    "Objects",
    "Particle",
    "PostProcess",
    "RenderPaths",
    "Scenes",
    "Scripts",
    "Sounds",
    "Shaders",
    "Techniques",
    "Textures",
    "UI",
    0
};

static const SPtr<Resource> noResource;

ResourceCache::ResourceCache() :
    autoReloadResources_(false),
    returnFailedResources_(false),
    searchPackagesFirst_(true),
    isRouting_(false),
    finishBackgroundResourcesMs_(5)
{
    // Create resource background loader. Its thread will start on the first background request
    backgroundLoader_ = new BackgroundLoader(this);

    // Subscribe BeginFrame for handling directory watchers and background loaded resource finalization
    Subscribe(&ResourceCache::HandleBeginFrame);

//	RegisterReader(new TextureReader());
}

ResourceCache::~ResourceCache()
{
    // Shut down the background loader first
    backgroundLoader_.Reset();
}

bool ResourceCache::AddResourceDir(const String& pathName, unsigned priority)
{
    std::lock_guard<Mutex> lock(resourceMutex_);

    FileSystem& fileSystem = GetSubsystem<FileSystem>();
    if (!fileSystem.DirExists(pathName))
    {
        UNIQUE_LOGERROR("Could not open directory " + pathName);
        return false;
    }

    // Convert path to absolute
    String fixedPath = SanitateResourceDirName(pathName);

    // Check that the same path does not already exist
    for (unsigned i = 0; i < resourceDirs_.size(); ++i)
    {
        if (!resourceDirs_[i].Compare(fixedPath, false))
            return true;
    }

    if (priority < resourceDirs_.size())
        resourceDirs_.insert(resourceDirs_.begin() + priority, fixedPath);
    else
        resourceDirs_.push_back(fixedPath);

    // If resource auto-reloading active, create a file watcher for the directory
    if (autoReloadResources_)
    {
        SPtr<FileWatcher> watcher(new FileWatcher());
        watcher->StartWatching(fixedPath, true);
        fileWatchers_.push_back(watcher);
    }

    UNIQUE_LOGINFO("Added resource path " + fixedPath);
    return true;
}

bool ResourceCache::AddPackageFile(PackageFile* package, unsigned priority)
{
	std::lock_guard<Mutex> lock(resourceMutex_);

    // Do not add packages that failed to load
    if (!package || !package->GetNumFiles())
    {
        UNIQUE_LOGERRORF("Could not add package file %s due to load failure", package->GetName().CString());
        return false;
    }

    if (priority < packages_.size())
        packages_.insert(packages_.begin() + priority, SPtr<PackageFile>(package));
    else
        packages_.push_back(SPtr<PackageFile>(package));

    UNIQUE_LOGINFO("Added resource package " + package->GetName());
    return true;
}

bool ResourceCache::AddPackageFile(const String& fileName, unsigned priority)
{
    SPtr<PackageFile> package(new PackageFile());
    return package->Open(fileName) && AddPackageFile(package);
}

bool ResourceCache::AddManualResource(Resource* resource)
{
    if (!resource)
    {
        UNIQUE_LOGERROR("Null manual resource");
        return false;
    }

    const String& name = resource->GetName();
    if (name.Empty())
    {
        UNIQUE_LOGERROR("Manual resource with empty name, can not add");
        return false;
    }

    resource->ResetUseTimer();
    resourceGroups_[resource->GetType()].resources_[resource->GetNameHash()] = resource;
    UpdateResourceGroup(resource->GetType());
    return true;
}

void ResourceCache::RemoveResourceDir(const String& pathName)
{
	std::lock_guard<Mutex> lock(resourceMutex_);

    String fixedPath = SanitateResourceDirName(pathName);

    for (unsigned i = 0; i < resourceDirs_.size(); ++i)
    {
        if (!resourceDirs_[i].Compare(fixedPath, false))
        {
            resourceDirs_.erase(resourceDirs_.begin() + i);
            // Remove the filewatcher with the matching path
            for (unsigned j = 0; j < fileWatchers_.size(); ++j)
            {
                if (!fileWatchers_[j]->GetPath().Compare(fixedPath, false))
                {
                    fileWatchers_.erase(fileWatchers_.begin() + j);
                    break;
                }
            }
            UNIQUE_LOGINFO("Removed resource path " + fixedPath);
            return;
        }
    }
}

void ResourceCache::RemovePackageFile(PackageFile* package, bool releaseResources, bool forceRelease)
{
	std::lock_guard<Mutex> lock(resourceMutex_);

    for (Vector<SPtr<PackageFile> >::iterator i = packages_.begin(); i != packages_.end(); ++i)
    {
        if (*i == package)
        {
            if (releaseResources)
                ReleasePackageResources(*i, forceRelease);
            UNIQUE_LOGINFO("Removed resource package " + (*i)->GetName());
            packages_.erase(i);
            return;
        }
    }
}

void ResourceCache::RemovePackageFile(const String& fileName, bool releaseResources, bool forceRelease)
{
	std::lock_guard<Mutex> lock(resourceMutex_);

    // Compare the name and extension only, not the path
    String fileNameNoPath = GetFileNameAndExtension(fileName);

    for (Vector<SPtr<PackageFile> >::iterator i = packages_.begin(); i != packages_.end(); ++i)
    {
        if (!GetFileNameAndExtension((*i)->GetName()).Compare(fileNameNoPath, false))
        {
            if (releaseResources)
                ReleasePackageResources(*i, forceRelease);
            UNIQUE_LOGINFO("Removed resource package " + (*i)->GetName());
            packages_.erase(i);
            return;
        }
    }
}

void ResourceCache::ReleaseResource(StringID type, const String& name, bool force)
{
    StringID nameHash(name);
    const SPtr<Resource>& existingRes = FindResource(type, nameHash);
    if (!existingRes)
        return;

    // If other references exist, do not release, unless forced
    if ((existingRes.Refs() == 1 && existingRes.WeakRefs() == 0) || force)
    {
        resourceGroups_[type].resources_.erase(nameHash);
        UpdateResourceGroup(type);
    }
}

void ResourceCache::ReleaseResources(StringID type, bool force)
{
    bool released = false;

    auto i = resourceGroups_.find(type);
    if (i != resourceGroups_.end())
    {
        for (auto j = i->second.resources_.begin();
             j != i->second.resources_.end();)
        {
            auto current = j++;
            // If other references exist, do not release, unless forced
            if ((current->second.Refs() == 1 && current->second.WeakRefs() == 0) || force)
            {
                i->second.resources_.erase(current);
                released = true;
            }
        }
    }

    if (released)
        UpdateResourceGroup(type);
}

void ResourceCache::ReleaseResources(StringID type, const String& partialName, bool force)
{
    bool released = false;

    auto i = resourceGroups_.find(type);
    if (i != resourceGroups_.end())
    {
        for (auto j = i->second.resources_.begin();
             j != i->second.resources_.end();)
        {
            auto current = j++;
            if (current->second->GetName().Contains(partialName))
            {
                // If other references exist, do not release, unless forced
                if ((current->second.Refs() == 1 && current->second.WeakRefs() == 0) || force)
                {
                    i->second.resources_.erase(current);
                    released = true;
                }
            }
        }
    }

    if (released)
        UpdateResourceGroup(type);
}

void ResourceCache::ReleaseResources(const String& partialName, bool force)
{
    // Some resources refer to others, like materials to textures. Release twice to ensure these get released.
    // This is not necessary if forcing release
    unsigned repeat = force ? 1 : 2;

    while (repeat--)
    {
        for (auto i = resourceGroups_.begin(); i != resourceGroups_.end(); ++i)
        {
            bool released = false;

            for (auto j = i->second.resources_.begin();
                 j != i->second.resources_.end();)
            {
                auto current = j++;
                if (current->second->GetName().Contains(partialName))
                {
                    // If other references exist, do not release, unless forced
                    if ((current->second.Refs() == 1 && current->second.WeakRefs() == 0) || force)
                    {
                        i->second.resources_.erase(current);
                        released = true;
                    }
                }
            }
            if (released)
                UpdateResourceGroup(i->first);
        }
    }
}

void ResourceCache::ReleaseAllResources(bool force)
{
    unsigned repeat = force ? 1 : 2;

    while (repeat--)
    {
        for (auto i = resourceGroups_.begin();
             i != resourceGroups_.end(); ++i)
        {
            bool released = false;

            for (auto j = i->second.resources_.begin();
                 j != i->second.resources_.end();)
            {
                auto current = j++;
                // If other references exist, do not release, unless forced
                if ((current->second.Refs() == 1 && current->second.WeakRefs() == 0) || force)
                {
                    i->second.resources_.erase(current);
                    released = true;
                }
            }
            if (released)
                UpdateResourceGroup(i->first);
        }
    }
}

bool ResourceCache::ReloadResource(Resource* resource)
{
    if (!resource)
        return false;

    resource->SendEvent(ReloadStarted());

    bool success = false;
    SPtr<File> file = GetFile(resource->GetName());
    if (file)
        success = resource->Load();

    if (success)
    {
        resource->ResetUseTimer();
        UpdateResourceGroup(resource->GetType());
        resource->SendEvent(ReloadFinished());
        return true;
    }

    // If reloading failed, do not remove the resource from cache, to allow for a new live edit to
    // attempt loading again
    resource->SendEvent(ReloadFailed());
    return false;
}

void ResourceCache::ReloadResourceWithDependencies(const String& fileName)
{
    StringID fileNameHash(fileName);
    // If the filename is a resource we keep track of, reload it
    const SPtr<Resource>& resource = FindResource(fileNameHash);
    if (resource)
    {
        UNIQUE_LOGDEBUG("Reloading changed resource " + fileName);
        ReloadResource(resource);
    }
    // Always perform dependency resource check for resource loaded from XML file as it could be used in inheritance
    if (!resource || GetExtension(resource->GetName()) == ".xml")
    {
        // Check if this is a dependency resource, reload dependents
        HashMap<StringID, HashSet<StringID> >::const_iterator j = dependentResources_.find(fileNameHash);
        if (j != dependentResources_.end())
        {
            // Reloading a resource may modify the dependency tracking structure. Therefore collect the
            // resources we need to reload first
            Vector<SPtr<Resource> > dependents;
            dependents.reserve((uint)j->second.size());

            for (HashSet<StringID>::const_iterator k = j->second.begin(); k != j->second.end(); ++k)
            {
                const SPtr<Resource>& dependent = FindResource(*k);
                if (dependent)
                    dependents.push_back(dependent);
            }

            for (unsigned k = 0; k < dependents.size(); ++k)
            {
                UNIQUE_LOGDEBUG("Reloading resource " + dependents[k]->GetName() + " depending on " + fileName);
                ReloadResource(dependents[k]);
            }
        }
    }
}

void ResourceCache::SetMemoryBudget(StringID type, unsigned long long budget)
{
    resourceGroups_[type].memoryBudget_ = budget;
}

void ResourceCache::SetAutoReloadResources(bool enable)
{
    if (enable != autoReloadResources_)
    {
        if (enable)
        {
            for (unsigned i = 0; i < resourceDirs_.size(); ++i)
            {
                SPtr<FileWatcher> watcher(new FileWatcher());
                watcher->StartWatching(resourceDirs_[i], true);
                fileWatchers_.push_back(watcher);
            }
        }
        else
            fileWatchers_.clear();

        autoReloadResources_ = enable;
    }
}

void ResourceCache::AddResourceRouter(ResourceRouter* router, bool addAsFirst)
{
    // Check for duplicate
    for (unsigned i = 0; i < resourceRouters_.size(); ++i)
    {
        if (resourceRouters_[i] == router)
            return;
    }

    if (addAsFirst)
        resourceRouters_.insert(resourceRouters_.begin(), SPtr<ResourceRouter>(router));
    else
        resourceRouters_.push_back(SPtr<ResourceRouter>(router));
}

void ResourceCache::RemoveResourceRouter(ResourceRouter* router)
{
    for (unsigned i = 0; i < resourceRouters_.size(); ++i)
    {
        if (resourceRouters_[i] == router)
        {
            resourceRouters_.erase(resourceRouters_.begin() + i);
            return;
        }
    }
}

SPtr<File> ResourceCache::GetFile(const String& nameIn, bool sendEventOnFailure)
{
    std::lock_guard<Mutex> lock(resourceMutex_);

    String name = SanitateResourceName(nameIn);
    if (!isRouting_)
    {
        isRouting_ = true;
        for (unsigned i = 0; i < resourceRouters_.size(); ++i)
            resourceRouters_[i]->Route(name, RESOURCE_GETFILE);
        isRouting_ = false;
    }

    if (name.Length())
    {
        File* file = 0;

        if (searchPackagesFirst_)
        {
            file = SearchPackages(name);
            if (!file)
                file = SearchResourceDirs(name);
        }
        else
        {
            file = SearchResourceDirs(name);
            if (!file)
                file = SearchPackages(name);
        }

        if (file)
            return SPtr<File>(file);
    }

    if (sendEventOnFailure)
    {
        if (resourceRouters_.size() && name.Empty() && !nameIn.Empty())
            UNIQUE_LOGERROR("Resource request " + nameIn + " was blocked");
        else
            UNIQUE_LOGERROR("Could not find resource " + name);

        if (Thread::IsMainThread())
        {
            //using namespace ResourceNotFound;

            //VariantMap& eventData = GetEventDataMap();
            //eventData[P_RESOURCENAME] = name.Length() ? name : nameIn;
			ResourceNotFound eventData;
			eventData.resourceName_ = name.Length() ? name : nameIn;
            SendEvent(eventData);
        }
    }

    return SPtr<File>();
}

Resource* ResourceCache::GetExistingResource(StringID type, const String& nameIn)
{
    String name = SanitateResourceName(nameIn);

    if (!Thread::IsMainThread())
    {
        UNIQUE_LOGERROR("Attempted to get resource " + name + " from outside the main thread");
        return 0;
    }

    // If empty name, return null pointer immediately
    if (name.Empty())
        return 0;

    StringID nameHash(name);

    const SPtr<Resource>& existing = FindResource(type, nameHash);
    return existing;
}

Resource* ResourceCache::GetResource(StringID type, const String& nameIn, bool sendEventOnFailure)
{
    String name = SanitateResourceName(nameIn);

    if (!Thread::IsMainThread())
    {
        UNIQUE_LOGERROR("Attempted to get resource " + name + " from outside the main thread");
        return 0;
    }

    // If empty name, return null pointer immediately
    if (name.Empty())
        return 0;

    StringID nameHash(name);

    // Check if the resource is being background loaded but is now needed immediately
    backgroundLoader_->WaitForResource(type, nameHash);

    const SPtr<Resource>& existing = FindResource(type, nameHash);
    if (existing)
        return existing;

    SPtr<Resource> resource;
	/*
    // Make sure the pointer is non-null and is a Resource subclass
    resource = DynamicCast<Resource>(context_->CreateObject(type));
    if (!resource)
    {
        UNIQUE_LOGERROR("Could not load unknown resource type " + String(type));

        if (sendEventOnFailure)
        {
            using namespace UnknownResourceType;

            VariantMap& eventData = GetEventDataMap();
            eventData[P_RESOURCETYPE] = type;
            SendEvent(E_UNKNOWNRESOURCETYPE, eventData);
        }

        return 0;
    }

    // Attempt to load the resource
    SPtr<File> file = GetFile(name, sendEventOnFailure);
    if (!file)
        return 0;   // Error is already logged

    UNIQUE_LOGDEBUG("Loading resource " + name);
    resource->SetName(name);*/

    //if (!resource->Load(*(file.Get())))

	resource = LoadResource(type, name);
	if (!resource)
    {
        // Error should already been logged by corresponding resource descendant class
        if (sendEventOnFailure)
        {
			LoadFailed eventData;
			eventData.resourceName_ = name;
            SendEvent(eventData);
        }

        if (!returnFailedResources_)
            return 0;
    }

	resource->SetName(name);
    // Store to cache
    resource->ResetUseTimer();
    resourceGroups_[type].resources_[nameHash] = resource;
    UpdateResourceGroup(type);

    return resource;
}

bool ResourceCache::BackgroundLoadResource(StringID type, const String& nameIn, bool sendEventOnFailure, Resource* caller)
{
    // If empty name, fail immediately
    String name = SanitateResourceName(nameIn);
    if (name.Empty())
        return false;

    // First check if already exists as a loaded resource
    StringID nameHash(name);
    if (FindResource(type, nameHash) != noResource)
        return false;

    return backgroundLoader_->QueueResource(type, name, sendEventOnFailure, caller);

}

SPtr<Resource> ResourceCache::GetTempResource(StringID type, const String& nameIn, bool sendEventOnFailure)
{
    String name = SanitateResourceName(nameIn);

    // If empty name, return null pointer immediately
    if (name.Empty())
        return SPtr<Resource>();

    SPtr<Resource> resource;
	/*
    // Make sure the pointer is non-null and is a Resource subclass
    resource = DynamicCast<Resource>(context_->CreateObject(type));
    if (!resource)
    {
        UNIQUE_LOGERROR("Could not load unknown resource type " + String(type));

        if (sendEventOnFailure)
        {
            using namespace UnknownResourceType;

            VariantMap& eventData = GetEventDataMap();
            eventData[P_RESOURCETYPE] = type;
            SendEvent(E_UNKNOWNRESOURCETYPE, eventData);
        }

        return SPtr<Resource>();
    }

    // Attempt to load the resource
    SPtr<File> file = GetFile(name, sendEventOnFailure);
    if (!file)
        return SPtr<Resource>();  // Error is already logged

    UNIQUE_LOGDEBUG("Loading temporary resource " + name);
    resource->SetName(file->GetName());*/

	resource = LoadResource(type, name);
	if (!resource)
	{
        // Error should already been logged by corresponding resource descendant class
        if (sendEventOnFailure)
        {
			LoadFailed eventData;
			eventData.resourceName_ = name;
            SendEvent(eventData);
        }

        return SPtr<Resource>();
    }

	resource->SetName(name);
    return resource;
}

unsigned ResourceCache::GetNumBackgroundLoadResources() const
{
    return backgroundLoader_->GetNumQueuedResources();
}

void ResourceCache::GetResources(PODVector<Resource*>& result, StringID type) const
{
    result.clear();
    auto i = resourceGroups_.find(type);
    if (i != resourceGroups_.end())
    {
        for (HashMap<StringID, SPtr<Resource> >::const_iterator j = i->second.resources_.begin();
             j != i->second.resources_.end(); ++j)
            result.push_back(j->second);
    }
}

bool ResourceCache::Exists(const String& nameIn) const
{
    std::lock_guard<Mutex> lock(resourceMutex_);

    String name = SanitateResourceName(nameIn);
    if (!isRouting_)
    {
        isRouting_ = true;
        for (unsigned i = 0; i < resourceRouters_.size(); ++i)
            resourceRouters_[i]->Route(name, RESOURCE_CHECKEXISTS);
        isRouting_ = false;
    }

    if (name.Empty())
        return false;

    for (unsigned i = 0; i < packages_.size(); ++i)
    {
        if (packages_[i]->Exists(name))
            return true;
    }

    FileSystem& fileSystem = GetSubsystem<FileSystem>();
    for (unsigned i = 0; i < resourceDirs_.size(); ++i)
    {
        if (fileSystem.FileExists(resourceDirs_[i] + name))
            return true;
    }

    // Fallback using absolute path
    return fileSystem.FileExists(name);
}

unsigned long long ResourceCache::GetMemoryBudget(StringID type) const
{
    auto i = resourceGroups_.find(type);
    return i != resourceGroups_.end() ? i->second.memoryBudget_ : 0;
}

unsigned long long ResourceCache::GetMemoryUse(StringID type) const
{
    auto i = resourceGroups_.find(type);
    return i != resourceGroups_.end() ? i->second.memoryUse_ : 0;
}

unsigned long long ResourceCache::GetTotalMemoryUse() const
{
    unsigned long long total = 0;
    for (auto i = resourceGroups_.begin(); i != resourceGroups_.end(); ++i)
        total += i->second.memoryUse_;
    return total;
}

String ResourceCache::GetResourceFileName(const String& name) const
{
    FileSystem& fileSystem = GetSubsystem<FileSystem>();
    for (unsigned i = 0; i < resourceDirs_.size(); ++i)
    {
        if (fileSystem.FileExists(resourceDirs_[i] + name))
            return resourceDirs_[i] + name;
    }

    if (IsAbsolutePath(name) && fileSystem.FileExists(name))
        return name;
    else
        return String();
}

ResourceRouter* ResourceCache::GetResourceRouter(unsigned index) const
{
    return index < resourceRouters_.size() ? resourceRouters_[index] : (ResourceRouter*)0;
}

String ResourceCache::GetPreferredResourceDir(const String& path) const
{
    String fixedPath = AddTrailingSlash(path);

    bool pathHasKnownDirs = false;
    bool parentHasKnownDirs = false;

    FileSystem& fileSystem = GetSubsystem<FileSystem>();

    for (unsigned i = 0; checkDirs[i] != 0; ++i)
    {
        if (fileSystem.DirExists(fixedPath + checkDirs[i]))
        {
            pathHasKnownDirs = true;
            break;
        }
    }
    if (!pathHasKnownDirs)
    {
        String parentPath = GetParentPath(fixedPath);
        for (unsigned i = 0; checkDirs[i] != 0; ++i)
        {
            if (fileSystem.DirExists(parentPath + checkDirs[i]))
            {
                parentHasKnownDirs = true;
                break;
            }
        }
        // If path does not have known subdirectories, but the parent path has, use the parent instead
        if (parentHasKnownDirs)
            fixedPath = parentPath;
    }

    return fixedPath;
}

String ResourceCache::SanitateResourceName(const String& nameIn) const
{
    // Sanitate unsupported constructs from the resource name
    String name = GetInternalPath(nameIn);
    name.Replace("../", "");
    name.Replace("./", "");

    // If the path refers to one of the resource directories, normalize the resource name
    FileSystem& fileSystem = GetSubsystem<FileSystem>();
    if (resourceDirs_.size())
    {
        String namePath = GetPath(name);
        String exePath = fileSystem.GetProgramDir().Replaced("/./", "/");
        for (unsigned i = 0; i < resourceDirs_.size(); ++i)
        {
            String relativeResourcePath = resourceDirs_[i];
            if (relativeResourcePath.StartsWith(exePath))
                relativeResourcePath = relativeResourcePath.Substring(exePath.Length());

            if (namePath.StartsWith(resourceDirs_[i], false))
                namePath = namePath.Substring(resourceDirs_[i].Length());
            else if (namePath.StartsWith(relativeResourcePath, false))
                namePath = namePath.Substring(relativeResourcePath.Length());
        }

        name = namePath + GetFileNameAndExtension(name);
    }

    return name.Trimmed();
}

String ResourceCache::SanitateResourceDirName(const String& nameIn) const
{
    String fixedPath = AddTrailingSlash(nameIn);
    if (!IsAbsolutePath(fixedPath))
        fixedPath = GetSubsystem<FileSystem>().GetCurrentDir() + fixedPath;

    // Sanitate away /./ construct
    fixedPath.Replace("/./", "/");

    return fixedPath.Trimmed();
}

void ResourceCache::StoreResourceDependency(Resource* resource, const String& dependency)
{
    if (!resource)
        return;

    std::lock_guard<Mutex> lock(resourceMutex_);

    StringID nameHash(resource->GetName());
    HashSet<StringID>& dependents = dependentResources_[dependency];
    dependents.insert(nameHash);
}

void ResourceCache::ResetDependencies(Resource* resource)
{
    if (!resource)
        return;

    std::lock_guard<Mutex> lock(resourceMutex_);

    StringID nameHash(resource->GetName());

    for (HashMap<StringID, HashSet<StringID> >::iterator i = dependentResources_.begin(); i != dependentResources_.end();)
    {
        HashSet<StringID>& dependents = i->second;
        dependents.erase(nameHash);
        if (dependents.empty())
            i = dependentResources_.erase(i);
        else
            ++i;
    }
}

String GetFileSizeString(unsigned long long memorySize)
{
	static const char* memorySizeStrings = "kMGTPE";

	String output;

	if (memorySize < 1024)
	{
		output = String(memorySize) + " b";
	}
	else
	{
		const int exponent = (int)(log((double)memorySize) / log(1024.0));
		const double majorValue = ((double)memorySize) / pow(1024.0, exponent);
		char buffer[64];
		memset(buffer, 0, 64);
		sprintf(buffer, "%.1f", majorValue);
		output = buffer;
		output += " ";
		output += memorySizeStrings[exponent - 1];
	}

	return output;
}

String ResourceCache::PrintMemoryUsage() const
{
    String output = "Resource Type                 Cnt       Avg       Max    Budget     Total\n\n";
    char outputLine[256];

    unsigned totalResourceCt = 0;
    unsigned long long totalLargest = 0;
    unsigned long long totalAverage = 0;
    unsigned long long totalUse = GetTotalMemoryUse();

    for (auto cit = resourceGroups_.begin(); cit != resourceGroups_.end(); ++cit)
    {
        const unsigned resourceCt = (unsigned)cit->second.resources_.size();
        unsigned long long average = 0;
        if (resourceCt > 0)
            average = cit->second.memoryUse_ / resourceCt;
        else
            average = 0;
        unsigned long long largest = 0;
        for (HashMap<StringID, SPtr<Resource> >::const_iterator resIt = cit->second.resources_.begin(); resIt != cit->second.resources_.end(); ++resIt)
        {
            if (resIt->second->GetMemoryUse() > largest)
                largest = resIt->second->GetMemoryUse();
            if (largest > totalLargest)
                totalLargest = largest;
        }

        totalResourceCt += resourceCt;

        const String countString(cit->second.resources_.size());
        const String memUseString = GetFileSizeString(average);
        const String memMaxString = GetFileSizeString(largest);
        const String memBudgetString = GetFileSizeString(cit->second.memoryBudget_);
        const String memTotalString = GetFileSizeString(cit->second.memoryUse_);
        const String resTypeName = cit->first.ToString();

        memset(outputLine, ' ', 256);
        outputLine[255] = 0;
        sprintf(outputLine, "%-28s %4s %9s %9s %9s %9s\n", resTypeName.CString(), countString.CString(), memUseString.CString(), memMaxString.CString(), memBudgetString.CString(), memTotalString.CString());

        output += ((const char*)outputLine);
    }

    if (totalResourceCt > 0)
        totalAverage = totalUse / totalResourceCt;

    const String countString(totalResourceCt);
    const String memUseString = GetFileSizeString(totalAverage);
    const String memMaxString = GetFileSizeString(totalLargest);
    const String memTotalString = GetFileSizeString(totalUse);

    memset(outputLine, ' ', 256);
    outputLine[255] = 0;
    sprintf(outputLine, "%-28s %4s %9s %9s %9s %9s\n", "All", countString.CString(), memUseString.CString(), memMaxString.CString(), "-", memTotalString.CString());
    output += ((const char*)outputLine);

    return output;
}

const SPtr<Resource>& ResourceCache::FindResource(StringID type, StringID nameHash)
{
    std::lock_guard<Mutex> lock(resourceMutex_);

    auto i = resourceGroups_.find(type);
    if (i == resourceGroups_.end())
        return noResource;
    auto j = i->second.resources_.find(nameHash);
    if (j == i->second.resources_.end())
        return noResource;

    return j->second;
}

const SPtr<Resource>& ResourceCache::FindResource(StringID nameHash)
{
    std::lock_guard<Mutex> lock(resourceMutex_);

    for (auto i = resourceGroups_.begin(); i != resourceGroups_.end(); ++i)
    {
        auto j = i->second.resources_.find(nameHash);
        if (j != i->second.resources_.end())
            return j->second;
    }

    return noResource;
}

void ResourceCache::ReleasePackageResources(PackageFile* package, bool force)
{
    HashSet<StringID> affectedGroups;

    const HashMap<String, PackageEntry>& entries = package->GetEntries();
    for (auto i = entries.begin(); i != entries.end(); ++i)
    {
        StringID nameHash(i->first);

        // We do not know the actual resource type, so search all type containers
        for (auto j = resourceGroups_.begin(); j != resourceGroups_.end(); ++j)
        {
            auto k = j->second.resources_.find(nameHash);
            if (k != j->second.resources_.end())
            {
                // If other references exist, do not release, unless forced
                if ((k->second.Refs() == 1 && k->second.WeakRefs() == 0) || force)
                {
                    j->second.resources_.erase(k);
                    affectedGroups.insert(j->first);
                }
                break;
            }
        }
    }

    for (auto i = affectedGroups.begin(); i != affectedGroups.end(); ++i)
        UpdateResourceGroup(*i);
}

void ResourceCache::UpdateResourceGroup(StringID type)
{
    auto i = resourceGroups_.find(type);
    if (i == resourceGroups_.end())
        return;

    for (;;)
    {
        unsigned totalSize = 0;
        unsigned oldestTimer = 0;
        auto oldestResource = i->second.resources_.end();

        for (auto j = i->second.resources_.begin();
             j != i->second.resources_.end(); ++j)
        {
            totalSize += j->second->GetMemoryUse();
            unsigned useTimer = j->second->GetUseTimer();
            if (useTimer > oldestTimer)
            {
                oldestTimer = useTimer;
                oldestResource = j;
            }
        }

        i->second.memoryUse_ = totalSize;

        // If memory budget defined and is exceeded, remove the oldest resource and loop again
        // (resources in use always return a zero timer and can not be removed)
        if (i->second.memoryBudget_ && i->second.memoryUse_ > i->second.memoryBudget_ &&
            oldestResource != i->second.resources_.end())
        {
            UNIQUE_LOGDEBUG("Resource group " + oldestResource->second->GetType().ToString() + " over memory budget, releasing resource " +
                     oldestResource->second->GetName());
            i->second.resources_.erase(oldestResource);
        }
        else
            break;
    }
}

void ResourceCache::HandleBeginFrame(const BeginFrame& eventData)
{
    for (unsigned i = 0; i < fileWatchers_.size(); ++i)
    {
        String fileName;
        while (fileWatchers_[i]->GetNextChange(fileName))
        {
            ReloadResourceWithDependencies(fileName);

            // Finally send a general file changed event even if the file was not a tracked resource
			FileChanged eventData;
			eventData.fileName_ = fileWatchers_[i]->GetPath() + fileName;
			eventData.resourceName_ = fileName;
            SendEvent(eventData);
        }
    }

    // Check for background loaded resources that can be finished
    {
        UNIQUE_PROFILE(FinishBackgroundResources);
        backgroundLoader_->FinishResources(finishBackgroundResourcesMs_);
    }

}

File* ResourceCache::SearchResourceDirs(const String& nameIn)
{
    FileSystem& fileSystem = GetSubsystem<FileSystem>();
    for (unsigned i = 0; i < resourceDirs_.size(); ++i)
    {
        if (fileSystem.FileExists(resourceDirs_[i] + nameIn))
        {
            // Construct the file first with full path, then rename it to not contain the resource path,
            // so that the file's name can be used in further GetFile() calls (for example over the network)
            File* file(new File(resourceDirs_[i] + nameIn));
            file->SetName(nameIn);
            return file;
        }
    }

    // Fallback using absolute path
    if (fileSystem.FileExists(nameIn))
        return new File(nameIn);

    return 0;
}

File* ResourceCache::SearchPackages(const String& nameIn)
{
    for (unsigned i = 0; i < packages_.size(); ++i)
    {
        if (packages_[i]->Exists(nameIn))
            return new File(packages_[i], nameIn);
    }

    return 0;
}


void ResourceCache::RegisterImporter(ResourceImporter* importer)
{
	resourceImporters_[importer->GetResourceType()] = importer;
}

SPtr<Resource> ResourceCache::LoadResource(StringID type, const String& name)
{
	SPtr<Resource> resource;
	//todo : load from cache dir

	auto it = resourceImporters_.find(type);
	if (it != resourceImporters_.end())
	{
		resource = it->second->Import(name);
		if (resource)
		{
			resource->SetName(name);
			if (!resource->Load())
			{
				return SPtr<Resource>();
			}
		}

	}


	return resource;
}

}
