#pragma once
#include "Core/Object.h"

namespace Unique
{
	class Engine : public Object
	{
		uRTTI(Engine, Object)
	public:
		Engine();
		~Engine();
	};


}
