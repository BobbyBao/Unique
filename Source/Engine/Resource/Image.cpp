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

	bool Image::Load(File& file)
	{
		uint size = file.GetSize();
		SharedArrayPtr<byte> data(new byte[size]);
		if(file.Read(data, size) != size)
		{
			return false;
		}
			
		data_ = stbi_load_from_memory(data.Get(), size, &width, &height, &components, reqComponents);
		imageDesc_.buffer = data_;	
		imageDesc_.format = (components == 4 ? LLGL::ImageFormat::RGBA : LLGL::ImageFormat::RGB);
		imageDesc_.dataType = LLGL::DataType::UInt8;
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