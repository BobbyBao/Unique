#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class Resource : public Object
	{
		uRTTI(Resource, Object)
	public:
		Resource();
		~Resource();
	};

}

