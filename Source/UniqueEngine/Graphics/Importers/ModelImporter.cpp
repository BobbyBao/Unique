#include "Precompiled.h"
#include "ModelImporter.h"
#include "../Model.h"
#include "Animation/Animation.h"

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
		if (!file)
		{
			return nullptr;
		}

		SPtr<Model> model(new Model());
		model->Load(*file);
		return model;
	}


	AnimationImporter::AnimationImporter() : ResourceImporter(Animation::GetTypeStatic())
	{
	}

	AnimationImporter::~AnimationImporter()
	{
	}

	SPtr<Resource> AnimationImporter::Import(const String& path)
	{
		SPtr<File> file = cache.GetFile(path);
		if (!file)
		{
			return nullptr;
		}

		SPtr<Animation> anim(new Animation());
		anim->Load(*file);
		return anim;
	}
}
