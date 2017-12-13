#include "Precompiled.h"
#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


namespace Unique
{
	Image::Image()
	{
	}

	Image::~Image()
	{
		if (data_)
		{
			stbi_image_free(data_);
			data_ = nullptr;
		}
	}

	bool Image::Load(IFile& file)
	{
		ByteArray data = file.ReadAll();
		if(data.empty())
		{
			return false;
		}
			
		data_ = stbi_load_from_memory((byte*)data.data(), data.size(), &desc_.width, &desc_.height, &desc_.numComponents, reqComponents);
		return data_ != nullptr;
	}

	SPtr<Image> Image::LoadImage(const char* filename)
	{
		File file(filename);
		if (!file.IsOpen())
		{
			return nullptr;
		}

		SPtr<Image> image(new Image());
		if(!image->Load(file))
		{
			return nullptr;
		}

		return image;
	}

}