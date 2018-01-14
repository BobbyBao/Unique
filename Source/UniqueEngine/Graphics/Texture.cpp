#include "UniquePCH.h"
#include "Texture.h"
#include "Resource/Image.h"
#include <GraphicsUtilities.h>
#include <GraphicsAccessories.h>

using namespace Diligent;

namespace Unique
{

	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
	}

	bool Texture::Create(const TextureDesc& desc, Vector< Vector<byte> >&& texData)
	{
		desc_ = desc;
		Mips = std::move(texData);

		pSubResources.resize(Mips.size());
		uint height = desc_.Height;
		for (size_t i = 0; i < Mips.size(); i++)
		{
			TextureSubResData subRes;
			subRes.pData = Mips[i].data();
			subRes.Stride = (uint)Mips[i].size() / height;
			pSubResources[i] = subRes;
			height /= 2;
		}

		texData_.pSubResources = pSubResources.data();
		texData_.NumSubresources = desc_.MipLevels;
		return GPUObject::Create();
	}

	bool Texture::Create(const TextureDesc& desc, const TextureData& texData)
	{
		desc_ = desc;
		texData_ = texData;
		return GPUObject::Create();
	}

	bool Texture::Create(Image& img, const TextureLoadInfo& TexLoadInfo)
	{
		const auto& ImgDesc = img.GetDesc();

		desc_.Name = img.GetName().CString();
		desc_.Type = RESOURCE_DIM_TEX_2D;
		desc_.Width = ImgDesc.width;
		desc_.Height = ImgDesc.height;
		desc_.MipLevels = ComputeMipLevelsCount(desc_.Width, desc_.Height);
		//if (TexLoadInfo.MipLevels > 0)
		//	desc_.MipLevels = std::min(desc_.MipLevels, TexLoadInfo.MipLevels);
		desc_.Usage = TexLoadInfo.Usage;
		desc_.BindFlags = TexLoadInfo.BindFlags;
		desc_.Format = TexLoadInfo.Format;
		desc_.CPUAccessFlags = TexLoadInfo.CPUAccessFlags;
		auto ChannelDepth = ImgDesc.bitsPerPixel / ImgDesc.numComponents;

		uint NumComponents = ImgDesc.numComponents == 3 ? 4 : ImgDesc.numComponents;
		bool IsSRGB = (ImgDesc.numComponents >= 3 && ChannelDepth == 8) ? TexLoadInfo.IsSRGB : false;
		if (desc_.Format == TEX_FORMAT_UNKNOWN)
		{
			if (ChannelDepth == 8)
			{
				switch (NumComponents)
				{
				case 1: desc_.Format = TEX_FORMAT_R8_UNORM; break;
				case 2: desc_.Format = TEX_FORMAT_RG8_UNORM; break;
				case 4: desc_.Format = IsSRGB ? TEX_FORMAT_RGBA8_UNORM_SRGB : TEX_FORMAT_RGBA8_UNORM; break;
				default: UNIQUE_LOGERROR("Unexpected number of color channels (", ImgDesc.numComponents, ")");
				}
			}
			else if (ChannelDepth == 16)
			{
				switch (NumComponents)
				{
				case 1: desc_.Format = TEX_FORMAT_R16_UNORM; break;
				case 2: desc_.Format = TEX_FORMAT_RG16_UNORM; break;
				case 4: desc_.Format = TEX_FORMAT_RGBA16_UNORM; break;
				default: UNIQUE_LOGERROR("Unexpected number of color channels (", ImgDesc.numComponents, ")");
				}
			}
			else
				UNIQUE_LOGERROR("Unsupported color channel depth (", ChannelDepth, ")");
		}
		else
		{
			const auto& TexFmtDesc = GetTextureFormatAttribs(desc_.Format);
			if (TexFmtDesc.NumComponents != NumComponents)
				UNIQUE_LOGERROR("Incorrect number of components ", ImgDesc.numComponents, ") for texture format ", TexFmtDesc.Name);
			if (TexFmtDesc.ComponentSize != ChannelDepth / 8)
				UNIQUE_LOGERROR("Incorrect channel size ", ChannelDepth, ") for texture format ", TexFmtDesc.Name);
		}


		pSubResources.resize(desc_.MipLevels);
		Mips.resize(desc_.MipLevels);

		if (ImgDesc.numComponents == 3)
		{
			VERIFY_EXPR(NumComponents == 4);
			auto RGBAStride = ImgDesc.width * NumComponents * ChannelDepth / 8;
			RGBAStride = (RGBAStride + 3) & (-4);
			Mips[0].resize(RGBAStride * ImgDesc.height);
			pSubResources[0].pData = Mips[0].data();
			pSubResources[0].Stride = RGBAStride;
			if (ChannelDepth == 8)
				RGBToRGBA<Uint8>(img.GetData(), ImgDesc.rowStride,
					Mips[0].data(), RGBAStride,
					ImgDesc.width, ImgDesc.height);
			else if (ChannelDepth == 16)
				RGBToRGBA<Uint16>(img.GetData(), ImgDesc.rowStride,
					Mips[0].data(), RGBAStride,
					ImgDesc.width, ImgDesc.height);
		}
		else
		{
			pSubResources[0].pData = img.GetData();
			pSubResources[0].Stride = ImgDesc.rowStride;
		}

		auto MipWidth = desc_.Width;
		auto MipHeight = desc_.Height;
		for (uint m = 1; m < desc_.MipLevels; ++m)
		{
			auto CoarseMipWidth = std::max(MipWidth / 2u, 1u);
			auto CoarseMipHeight = std::max(MipHeight / 2u, 1u);
			auto CoarseMipStride = CoarseMipWidth * NumComponents * ChannelDepth / 8;
			CoarseMipStride = (CoarseMipStride + 3) & (-4);
			Mips[m].resize(CoarseMipStride * CoarseMipHeight);

			if (TexLoadInfo.GenerateMips)
			{
				if (ChannelDepth == 8)
					ComputeCoarseMip<Uint8>(NumComponents, IsSRGB,
						pSubResources[m - 1].pData, pSubResources[m - 1].Stride,
						Mips[m].data(), CoarseMipStride,
						CoarseMipWidth, CoarseMipHeight);
				else if (ChannelDepth == 16)
					ComputeCoarseMip<Uint16>(NumComponents, IsSRGB,
						pSubResources[m - 1].pData, pSubResources[m - 1].Stride,
						Mips[m].data(), CoarseMipStride,
						CoarseMipWidth, CoarseMipHeight);
			}

			pSubResources[m].pData = Mips[m].data();
			pSubResources[m].Stride = CoarseMipStride;

			MipWidth = CoarseMipWidth;
			MipHeight = CoarseMipHeight;
		}

		texData_.pSubResources = pSubResources.data();
		texData_.NumSubresources = desc_.MipLevels;

		return GPUObject::Create();
	}
	
	bool Texture::CreateImpl()
	{
		auto& graphics = GetSubsystem<Graphics>();
		graphics.CreateTexture(desc_, texData_, *this);
		CreateTextureView();
		return deviceObject_ != nullptr;
	}

	
	bool Texture::CreateTextureView()
	{
		auto& graphics = GetSubsystem<Graphics>();
		if (!deviceObject_)
		{
			return false;
		}

		graphics.CreateSampler(samplerDesc_, &sampler_);

		ITexture* textureObject = (ITexture*)deviceObject_;

		if (desc_.BindFlags & BIND_SHADER_RESOURCE)
		{
			shaderResourceView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
			shaderResourceView_->SetSampler(sampler_);
		}

		if (desc_.BindFlags & BIND_RENDER_TARGET)
		{
			renderTargetView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET));
			shaderResourceView_->SetSampler(sampler_);
		}

		if (desc_.BindFlags & BIND_DEPTH_STENCIL)
		{
			depthStencilView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL));
			shaderResourceView_->SetSampler(sampler_);
		}

		if (desc_.BindFlags & BIND_UNORDERED_ACCESS)
		{
			unorderedAccessView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS));
			shaderResourceView_->SetSampler(sampler_);
		}

		return true;
	}

	void Texture::ReleaseImpl()
	{
		GPUObject::ReleaseImpl();

		shaderResourceView_ = nullptr;
		renderTargetView_ = nullptr;
		depthStencilView_ = nullptr;
		unorderedAccessView_ = nullptr;
	}

	
}