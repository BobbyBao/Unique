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

	struct TextureLoadInfo
	{
		const Diligent::Char *Name;
		Diligent::USAGE Usage;
		Diligent::Uint32 BindFlags;
		Diligent::Uint32 MipLevels;
		Diligent::Uint32 CPUAccessFlags;
		Diligent::Bool IsSRGB;
		Diligent::Bool GenerateMips;
		Diligent::TEXTURE_FORMAT Format;

		TextureLoadInfo() :
			Name(""),
			Usage(Diligent::USAGE_STATIC),
			BindFlags(Diligent::BIND_SHADER_RESOURCE),
			MipLevels(0),
			CPUAccessFlags(0),
			IsSRGB(false),
			GenerateMips(true),
			Format(Diligent::TEX_FORMAT_UNKNOWN)
		{}
	};

	class Texture : public Resource, public GPUObject
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();

		bool Create(const TextureDesc& desc, const TextureData& texData);

		bool Create(Image& img, const TextureLoadInfo& TexLoadInfo = TextureLoadInfo());

		uint GetWidth() const { return desc_.Width; }

		uint GetHeight() const { return desc_.Height; }

		TextureView* GetShaderResourceView() { return shaderResourceView_; }
		TextureView* GetRenderTargetView() { return renderTargetView_; }
		TextureView* GetDepthStencilView() { return depthStencilView_; }
		TextureView* GetUnorderedAccessView() { return unorderedAccessView_; }
	protected:
		bool CreateImpl();
		void ReleaseImpl();

		TextureDesc desc_;
		TextureData texData_;
		SPtr<TextureView> shaderResourceView_;
		SPtr<TextureView> renderTargetView_;
		SPtr<TextureView> depthStencilView_;
		SPtr<TextureView> unorderedAccessView_;
	};

	class TextureView : public RefCounted
	{
	public:
		TextureView(Texture& texture, ITextureView* textureView) 
			: texture_(texture), textureView_(textureView)
		{
		}

		uint GetWidth() const { return texture_.GetWidth(); }
		uint GetHeight() const { return texture_.GetHeight(); }
	private:
		Texture& texture_;
		ITextureView* textureView_;
	};

}

