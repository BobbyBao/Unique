#pragma once
#include "../Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GfxObject.h"
#include <LLGL/Texture.h>

namespace Unique
{
	class Image;

	class Texture : public Resource, public TGfxObject<LLGL::Texture>
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();

		void SetSampler(const SamplerDescriptor& desc);
		bool Create(Image& img);

		bool Save(const String& filename, unsigned int mipLevel = 0);

		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		static SPtr<Texture> Load(const String& filename);
	protected:
		TextureDescriptor textureDesc_;
		SamplerDescriptor samplerDesc_;
		Sampler* sampler_ = nullptr;

	};

	inline void Texture::SetSampler(const SamplerDescriptor& desc)
	{
		samplerDesc_ = desc;
	}

}

