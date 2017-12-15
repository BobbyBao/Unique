#pragma once
#include "Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GPUObject.h"


namespace Unique
{
	class Image;

	class Texture : public Resource, public GPUObject
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();


		//bool Create2D(int width, int height);

		bool Create(const TextureDesc& desc, const TextureData& texData);

		bool Create(Image& img);

		uint GetWidth() const { return desc_.Width; }
		uint GetHeight() const { return desc_.Height; }
	protected:
		bool CreateImpl();
		TextureDesc desc_;
		TextureData texData_;
	};

	class TextureView : public RefCounted
	{
	public:
		ITextureView* textureView_;
	};


}

