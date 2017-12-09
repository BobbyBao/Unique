#pragma once
#include "Resource/Resource.h"

namespace Diligent
{
	class IResourceMapping;
}

namespace Unique
{
	class Material : public Resource
	{
		uRTTI(Material, Resource)
	public:
		Material();
		~Material();

		Diligent::IResourceMapping* resourceMapping_;
	};


}
