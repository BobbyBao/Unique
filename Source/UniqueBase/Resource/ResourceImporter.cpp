#include "Precompiled.h"
#include "ResourceImporter.h"
#include "ResourceCache.h"
#include "../io/FileSystem.h"
#include "Serialize/JsonDeserializer.h"

namespace Unique
{
	ResourceImporter::ResourceImporter(StringID type) : type_(type), cache(GetSubsystem<ResourceCache>())
	{
		UNIQUE_LOGINFOF("Regiseter reader : %s", type);
	}

	ResourceImporter::~ResourceImporter()
	{
	}

	SPtr<Resource> ResourceImporter::Import(const String& filePath)
	{
		SPtr<Resource> res = DynamicCast<Resource>(GetContext()->CreateObject(type_));
		JsonDeserializer reader;
		reader.Load(filePath, res);
		return res;
	}

	
}
