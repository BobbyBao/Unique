#pragma once
#include "Core/Object.h"

namespace Unique
{
	class Input : public Object
	{
	public:
		Input();
		~Input();

		bool ProcessEvents();
	};

}

