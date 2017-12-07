#pragma once
#include "Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GPUObject.h"
#include <LLGL/Texture.h>

namespace Unique
{
	class Image;

	class Texture : public GPUObject<Resource, LLGL::Texture>
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();

		bool Create(Image& img);

		bool Save(const String& filename, unsigned int mipLevel = 0);

		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		static SPtr<Texture> Load(const String& filename);
	protected:
		LLGL::TextureDescriptor textureDesc_;

	};


}

