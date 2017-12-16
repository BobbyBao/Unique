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
		ITexture* textureObject = nullptr;
		renderDevice->CreateTexture(desc_, texData_, (ITexture**)&textureObject);
		if (!handle_)
		{
			return false;
		}

		handle_ = textureObject;
		TEXTURE_VIEW_TYPE viewType; 
		//BIND_SHADER_RESOURCE = 0x8L, ///< A buffer or a texture can be bound as a shader resource
									 ///  \warning This flag cannot be used with MAP_WRITE_NO_OVERWRITE flag 
		//	BIND_STREAM_OUTPUT = 0x10L,///< A buffer can be bound as a target for stream output stage
		//	BIND_RENDER_TARGET = 0x20L,///< A texture can be bound as a render target
		//	BIND_DEPTH_STENCIL = 0x40L,///< A texture can be bound as a depth-stencil target
		//	BIND_UNORDERED_ACCESS = 0x80L,///< A buffer or a texture can be bound as an unordered access view
		//	BIND_INDIRECT_DRAW_ARGS = 0x100L

		//textureObject->GetDefaultView(viewType);
		return handle_ != nullptr;
	}

	bool Texture::Create(Image& img)
	{


		return handle_ != nullptr;
	}
	
	
}