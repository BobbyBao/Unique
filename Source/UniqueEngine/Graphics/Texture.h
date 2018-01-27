#pragma once
#include "Resource/Resource.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GPUObject.h"
#include <RefCntAutoPtr.h>
#include <Texture.h>
#include <Sampler.h>


namespace Unique
{
	using ComponentType = Diligent::COMPONENT_TYPE;
	using TextureFormat = Diligent::TEXTURE_FORMAT;
	using TextureDesc = Diligent::TextureDesc;
	using TextureData = Diligent::TextureData;
	using SamplerDesc = Diligent::SamplerDesc;

	using TextureSubResData = Diligent::TextureSubResData;

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
			Usage(USAGE_STATIC),
			BindFlags(Diligent::BIND_SHADER_RESOURCE),
			MipLevels(0),
			CPUAccessFlags(0),
			IsSRGB(false),
			GenerateMips(true),
			Format(Diligent::TEX_FORMAT_UNKNOWN)
		{}
	};

	class Sampler : public GPUObject
	{
	public:
		SamplerDesc samplerDesc_;
	};

	class Texture : public Resource, public GPUObject
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();

		bool Create(const TextureDesc& desc, Vector< Vector<byte> >&& texData);
		bool Create(Image& img, const TextureLoadInfo& TexLoadInfo = TextureLoadInfo());
		bool Create(const TextureDesc& desc, const TextureData& texData);

		uint GetWidth() const { return desc_.Width; }
		uint GetHeight() const { return desc_.Height; }

		TextureView* GetShaderResourceView() { return shaderResourceView_; }
		TextureView* GetRenderTargetView() { return renderTargetView_; }
		TextureView* GetDepthStencilView() { return depthStencilView_; }
		TextureView* GetUnorderedAccessView() { return unorderedAccessView_; }
		Sampler&	GetSampler() { return sampler_; }
		
	protected:	
		bool CreateImpl();
		void ReleaseImpl();

		TextureDesc desc_;
		TextureData texData_;
		Sampler sampler_;

		SPtr<TextureView> shaderResourceView_;
		SPtr<TextureView> renderTargetView_;
		SPtr<TextureView> depthStencilView_;
		SPtr<TextureView> unorderedAccessView_;

		Vector<TextureSubResData> pSubResources;
		Vector< Vector<byte> > Mips;

		friend class Graphics;
	};

	class TextureView : public RefCounted
	{
	public:
		TextureView(Texture& texture, void* textureView) 
			: texture_(texture), textureView_(textureView)
		{
		}

		uint GetWidth() const { return texture_.GetWidth(); }
		uint GetHeight() const { return texture_.GetHeight(); }	
		
		template<class T>
		operator T*() {
			return (T*)textureView_;
		}

	private:
		Texture& texture_;
		void* textureView_;
	};

}

