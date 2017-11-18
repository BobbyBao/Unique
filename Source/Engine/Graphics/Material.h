#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class Material : public Object
	{
		UNIQUE_OBJECT(Material, Object)
	public:
		Material();
		~Material();
	};


}
