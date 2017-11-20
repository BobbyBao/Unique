#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	class Texture : public Resource
	{
		uRTTI(Texture, Resource)
	public:
		Texture();
		~Texture();
	};

}

