#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	struct ImageDesc
	{
		int width;
		int height;
		int bitsPerPixel;
		int numComponents;
		int rowStride; // In bytes
	};

	class Image : public Resource
	{
		uRTTI(Image, Resource)
	public:
		Image();
		~Image();

		bool Load(IStream& file);

		static SPtr<Image> LoadImage(const char* filename);
		
	protected:
		ImageDesc desc_;
		int reqComponents = 0;
		byte* data_ = nullptr;
	};

	class ImageImporter : public ResourceImporter
	{
	public:

	};

}
