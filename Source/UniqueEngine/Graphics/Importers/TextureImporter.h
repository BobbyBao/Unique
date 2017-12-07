#pragma once
#include "Resource/ResourceImporter.h"

namespace Unique
{

	class TextureImporter : public ResourceImporter
	{
	public:
		TextureImporter();

		virtual SPtr<Resource> Import(const String& path);
	private:
		SPtr<Resource> loadRaw(const String& path);
	};

}