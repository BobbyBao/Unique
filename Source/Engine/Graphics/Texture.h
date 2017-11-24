#pragma once
#include "../Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GfxObject.h"
#include <LLGL/Texture.h>

namespace Unique
{
	class Texture : public Resource, public TGPUObject<LLGL::Texture>
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();


		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		static SPtr<Texture> Load(const String& filename);
		// Save texture image to a PNG file.
		static bool Save(Texture& texture, const String& filename, unsigned int mipLevel = 0);

	};

}

