#include "Precompiled.h"
#include "ResourceImporter.h"
#include "ResourceCache.h"
#include "../io/FileSystem.h"
#include "../graphics/texture.h"

namespace Unique
{
	ResourceImporter::ResourceImporter(StringID type) : type_(type), cache(Subsystem<ResourceCache>())
	{
		UNIQUE_LOGINFOF("Regiseter reader : %s", type);
	}

	ResourceImporter::~ResourceImporter()
	{
	}

	SPtr<Resource> ResourceImporter::Import(const String& filePath)
	{
		return nullptr;
	}

	
}
