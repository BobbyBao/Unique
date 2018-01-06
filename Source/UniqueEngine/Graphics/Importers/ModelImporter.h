#pragma once

#include "resource/ResourceImporter.h"

namespace Unique
{

	class ModelImporter : public ResourceImporter
	{
	public:
		ModelImporter();
		~ModelImporter();

		virtual SPtr<Resource> Import(const String& path);
	private:

	};

	class AnimationImporter : public ResourceImporter
	{
	public:
		AnimationImporter();
		~AnimationImporter();

		virtual SPtr<Resource> Import(const String& path);
	private:

	};
}