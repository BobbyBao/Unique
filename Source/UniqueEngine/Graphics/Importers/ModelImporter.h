#pragma once

#include "resource/ResourceImporter.h"

namespace Unique
{
	class Model;

	class ModelImporter : public TResourceImporter<Model>
	{
	public:
		ModelImporter();
		~ModelImporter();

		virtual SPtr<Resource> Import(const String& filePath);
	private:
		SPtr<Resource> LoadObjFile(IStream& source);
		template<class Vertex>
		SPtr<Resource> LoadModel();

	};

	class AnimationImporter : public TResourceImporter<class Animation>
	{
	public:
		AnimationImporter();
		~AnimationImporter();

		virtual SPtr<Resource> Import(const String& path);
	private:

	};
}