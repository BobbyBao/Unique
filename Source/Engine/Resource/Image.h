#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	class Image : public Resource
	{
		uRTTI(Image, Resource)
	public:
		Image();
		~Image();

		static byte* LoadImage(const char* filename, int *x, int *y, int *comp, int req_comp);
		static void FreeImage(void *retval_from_stbi_load);
	};


}
