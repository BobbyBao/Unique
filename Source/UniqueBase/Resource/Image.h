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

		bool Load(File& file);

		static SPtr<Image> LoadImage(const char* filename);
		
		int width = 0;
		int height = 0;
	protected:
		byte* data_ = nullptr;

		int components = 0;
		int reqComponents = 0;
	};

	class ImageImporter : public ResourceImporter
	{
	public:

	};

}
