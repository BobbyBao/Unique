#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class Component : public Object
	{
		uRTTI(Component, Object)
	public:
		Component();
		~Component();
	};

}
