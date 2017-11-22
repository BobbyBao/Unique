#include "Precompiled.h"
#include "Texture.h"
#include "../Resource/Image.h"
#include <LLGL/Utility.h>

namespace Unique
{
	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
	}
	
	// Load image from file, create texture, upload image into texture, and generate MIP-maps.
	LLGL::Texture* Texture::Load(const String& filename)
	{
		int width = 0, height = 0, components = 0;
		unsigned char* imageBuffer = Image::LoadImage(filename.CString(), &width, &height, &components, 4);
		if (!imageBuffer)
			UNIQUE_LOGERROR("failed to load texture from file: \"" + filename + "\"");

		// Initialize image descriptor to upload image data onto hardware texture
		LLGL::ImageDescriptor imageDesc;
		{
			// Set image buffer color format
			imageDesc.format = LLGL::ImageFormat::RGBA;

			// Set image buffer data type (unsigned char = 8-bit unsigned integer)
			imageDesc.dataType = LLGL::DataType::UInt8;

			// Set image buffer source for texture initial data
			imageDesc.buffer = imageBuffer;
		}

		auto tex = renderer->CreateTexture(
			LLGL::Texture2DDesc(LLGL::TextureFormat::RGBA, width, height), &imageDesc
		);

		renderer->GenerateMips(*tex);

		// Release image data
		Image::FreeImage(imageBuffer);
		
		return tex;
	}
	
	bool Texture::Save(LLGL::Texture& texture, const String& filename, unsigned int mipLevel)
	{/*
		// Get texture dimension
		auto texSize = texture.QueryMipLevelSize(0).Cast<int>();

		// Read texture image data
		std::vector<LLGL::ColorRGBAub> imageBuffer(texSize.x*texSize.y);
		renderSys.ReadTexture(texture, mipLevel, LLGL::ImageFormat::RGBA, LLGL::DataType::UInt8, imageBuffer.data());

		// Save image data to file (using STBI library, see https://github.com/nothings/stb)
		if (!stbi_write_png(filename.c_str(), texSize.x, texSize.y, 4, imageBuffer.data(), texSize.x * 4))
		{
			std::cerr << "failed to write texture to file: \"" + filename + "\"" << std::endl;
			return false;
		}

		*/
		return true;
	}
}