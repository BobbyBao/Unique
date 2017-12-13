#include "UniquePCH.h"
#include "Texture.h"
#include "Resource/Image.h"

namespace Unique
{

	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
	}

	bool Texture::Create(const TextureDesc& desc, const TextureData& texData)
	{
		desc_ = desc;
		texData_ = texData;
		return GPUObject::Create();
	}

	bool Texture::CreateImpl()
	{
		renderDevice->CreateTexture(desc_, texData_, (ITexture**)&handle_);
		return handle_ != nullptr;
	}

	bool Texture::Create(Image& img)
	{


		return handle_ != nullptr;
	}
	
	
}