#pragma once
#include "Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GPUObject.h"
#include <RefCntAutoPtr.h>
#include <TextureView.h>


namespace Unique
{
	using TextureDesc = Diligent::TextureDesc;
	using TextureData = Diligent::TextureData;
	using SamplerDesc = Diligent::SamplerDesc;
	using ISampler = Diligent::ISampler;
	using TextureSubResData = Diligent::TextureSubResData;
	using ITextureView = Diligent::ITextureView;

	class Image;
	class TextureView;

	struct TextureLoadInfo
	{
		const char *Name;
		Usage Usage;
		uint BindFlags;
		uint MipLevels;
		uint CPUAccessFlags;
		bool IsSRGB;
		bool GenerateMips;
		TextureFormat Format;

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
		ISampler*	GetSampler() { return sampler_; }
	protected:
		bool CreateImpl();
		void ReleaseImpl();

		TextureDesc desc_;
		TextureData texData_;
		SamplerDesc samplerDesc_;
		ISampler*	sampler_ = nullptr;
		SPtr<TextureView> shaderResourceView_;
		SPtr<TextureView> renderTargetView_;
		SPtr<TextureView> depthStencilView_;
		SPtr<TextureView> unorderedAccessView_;

		Vector<TextureSubResData> pSubResources;
		Vector< Vector<byte> > Mips;
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
		
		operator ITextureView*() {
			return textureView_;
		}

		void SetSampler(ISampler* sampler) { textureView_->SetSampler(sampler); }
		ISampler* GetSampler() { return textureView_->GetSampler(); }
	private:
		Texture& texture_;
		ITextureView* textureView_;
	};

}

