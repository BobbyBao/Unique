#pragma once
#include "../Resource/Resource.h"
#include <LLGL/Image.h>

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
		
		const LLGL::ImageDescriptor& GetDesc() const { return imageDesc_; }
		
		int width = 0;
		int height = 0;
	protected:
		byte* data_ = nullptr;

		int components = 0;
		int reqComponents = 0;
		LLGL::ImageDescriptor imageDesc_;
	};

	class ImageImporter : public ResourceImporter
	{
	public:

	};

}
