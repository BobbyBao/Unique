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
	}

	byte* Image::LoadImage(const char* filename, int *x, int *y, int *comp, int req_comp)
	{
		return stbi_load(filename, x, y, comp, req_comp);
	}

	void Image::FreeImage(void *retval_from_stbi_load)
	{
		stbi_image_free(retval_from_stbi_load);
	}
}