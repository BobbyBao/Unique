#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	class Material : public Resource
	{
		uRTTI(Material, Resource)
	public:
		Material();
		~Material();
	};


}
