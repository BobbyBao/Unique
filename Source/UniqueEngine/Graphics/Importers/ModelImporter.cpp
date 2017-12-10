#include "Precompiled.h"
#include "ModelImporter.h"
#include "../Model.h"

namespace Unique
{
	ModelImporter::ModelImporter() : ResourceImporter(Model::GetTypeStatic())
	{
	}

	ModelImporter::~ModelImporter()
	{
	}

	SPtr<Resource> ModelImporter::Import(const String& path)
	{
		SPtr<File> file = cache.GetFile(path);
		if (file)
		{
			return nullptr;
		}

		SPtr<Model> model;

		return model;
	}
}
