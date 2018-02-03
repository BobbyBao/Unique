#pragma once
#include "Resource/ResourceImporter.h"
#include "../texture.h"

namespace Unique
{

	class TextureImporter : public ResourceImporter
	{
	public:
		TextureImporter();

		virtual SPtr<Resource> Import(const String& filePath);
	private:
		SPtr<Resource> LoadDDS(const String& filePath);
		TextureLoadInfo TexLoadInfo;
	};

}