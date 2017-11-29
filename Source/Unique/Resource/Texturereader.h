#pragma once
#include "../resource/ResourceReader.h"

namespace Unique
{

	class TextureReader : public ResourceReader
	{
	public:
		TextureReader();

		virtual SPtr<Resource> load(const String& path);
	private:
		SPtr<Resource> loadRaw(const String& path);
	};

}