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
		
	protected:
		bool CreateImpl();
		TextureDesc desc_;
		TextureData texData_;
	};


}

