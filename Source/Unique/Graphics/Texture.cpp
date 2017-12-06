#include "UniquePCH.h"
#include "Texture.h"
#include "Resource/Image.h"
#include <LLGL/Utility.h>

namespace Unique
{
	extern UPtr<LLGL::RenderSystem>        renderer;

	Texture::Texture() : sampler_(nullptr)
	{
	}

	Texture::~Texture()
	{
	}

	bool Texture::Create(Image& img)
	{
		textureDesc_ = LLGL::Texture2DDesc(LLGL::TextureFormat::RGBA, img.width, img.height);
		auto tex = renderer->CreateTexture(textureDesc_, &img.GetDesc());

		renderer->GenerateMips(*tex);
		sampler_ = renderer->CreateSampler(samplerDesc_);
		handle_ = tex;
		return handle_ != nullptr;
	}
	
	// Load image from file, create texture, upload image into texture, and generate MIP-maps.
	SPtr<Texture> Texture::Load(const String& filename)
	{
		SPtr<Image> img = Image::LoadImage(filename.CString());
		if (!img)
		{
			UNIQUE_LOGERROR("failed to load texture from file: \"" + filename + "\"");
		}

		SPtr<Texture> spTex = MakeShared<Texture>();
		spTex->Create(*img);
		return spTex;
	}
	
	bool Texture::Save(const String& filename, unsigned int mipLevel)
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