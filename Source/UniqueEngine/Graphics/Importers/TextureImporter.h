#pragma once
#include "Resource/ResourceImporter.h"
#include "../texture.h"

namespace Unique
{

	class TextureImporter : public ResourceImporter
	{
	public:
		TextureImporter();

		virtual SPtr<Resource> Import(const String& path);
	private:
		SPtr<Resource> LoadDDS(const String& path);
		TextureLoadInfo TexLoadInfo;
	};

}