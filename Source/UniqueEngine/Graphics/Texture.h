#pragma once
#include "Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GPUObject.h"
#include <RefCntAutoPtr.h>
#include <TextureView.h>


namespace Unique
{
	class Image;
	class TextureView;

	class Texture : public Resource, public GPUObject
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();

		bool Create(const TextureDesc& desc, const TextureData& texData);

		bool Create(Image& img);

		uint GetWidth() const { return desc_.Width; }

		uint GetHeight() const { return desc_.Height; }

		TextureView* GetTextureView() {
			return textureView_.get();
		}
	protected:
		bool CreateImpl();

		TextureDesc desc_;

		TextureData texData_;

		//textureObject->GetDefaultView()
		UPtr<TextureView> textureView_;
	};

	class TextureView
	{
	public:

		int GetWidth() const { return width_; }
		int GetHeight() const { return height_; }
	private:
		int width_;
		int height_;
		RefCntAutoPtr<ITextureView> textureView_;
	};

}

