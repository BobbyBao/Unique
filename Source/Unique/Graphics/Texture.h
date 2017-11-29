#pragma once
#include "../Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GfxObject.h"
#include <LLGL/Texture.h>

namespace Unique
{
	class Image;

	class Texture : public TGfxObject<Resource, LLGL::Texture>
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();

		void SetSampler(const LLGL::SamplerDescriptor& desc);
		bool Create(Image& img);

		bool Save(const String& filename, unsigned int mipLevel = 0);
		LLGL::Sampler& sampler() { return *sampler_;}
		// Load image from file, create texture, upload image into texture, and generate MIP-maps.
		static SPtr<Texture> Load(const String& filename);
	protected:
		LLGL::TextureDescriptor textureDesc_;
		LLGL::SamplerDescriptor samplerDesc_;
		LLGL::Sampler* sampler_ = nullptr;

	};

	inline void Texture::SetSampler(const LLGL::SamplerDescriptor& desc)
	{
		samplerDesc_ = desc;
	}

}

