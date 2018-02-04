#include "Precompiled.h"
#include "ResourceImporter.h"
#include "ResourceCache.h"
#include "../io/FileSystem.h"
#include "Serialize/JsonDeserializer.h"
#include "Serialize/BinaryDeserializer.h"

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
		if (filePath.EndsWith(".json", false))
		{
			JsonDeserializer jsonReader;
			jsonReader.Load(filePath, res);
		}
		else
		{
			BinaryDeserializer binaryReader;
			binaryReader.Load(filePath, res);
		}
		return res;
	}

	
}
