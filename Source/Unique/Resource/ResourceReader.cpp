#include "Precompiled.h"
#include "ResourceReader.h"
#include "ResourceCache.h"
#include "../io/FileSystem.h"
#include "../graphics/texture.h"

namespace Unique
{
	ResourceReader::ResourceReader(StringID type) : type_(type)
	{
		UNIQUE_LOGINFOF("Regiseter reader : %s", type);
	}


	ResourceReader::~ResourceReader()
	{
	}

	SPtr<Resource> ResourceReader::load(const String& filePath)
	{
		return nullptr;
	}

	
}
