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

	bool Texture::Create(Image& img)
	{

		return deviceObject_ != nullptr;
	}

	bool Texture::CreateImpl()
	{
		ITexture* textureObject = nullptr;
		renderDevice->CreateTexture(desc_, texData_, (ITexture**)&textureObject);
		deviceObject_ = textureObject;
		if (!deviceObject_)
		{
			return false;
		}

		if (desc_.BindFlags & BIND_SHADER_RESOURCE)
		{
			shaderResourceView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
		}

		if (desc_.BindFlags & BIND_RENDER_TARGET)
		{
			renderTargetView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET));
		}

		if (desc_.BindFlags & BIND_DEPTH_STENCIL)
		{
			depthStencilView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL));
		}

		if (desc_.BindFlags & BIND_UNORDERED_ACCESS)
		{
			unorderedAccessView_ = new TextureView(*this, textureObject->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS));
		}

		return deviceObject_ != nullptr;
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