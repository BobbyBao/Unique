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

}