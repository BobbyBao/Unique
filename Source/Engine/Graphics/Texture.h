#pragma once
#include "../Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"

namespace Unique
{
	class Texture : public Resource
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();


		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		static LLGL::Texture* Load(const String& filename);
		// Save texture image to a PNG file.
		static bool Save(LLGL::Texture& texture, const String& filename, unsigned int mipLevel = 0);

	};

}

