#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class Batch
	{
	public:


	};

	class View : public Object
	{
		uRTTI(View, Object)
	public:
		View();
		~View();

		void Render();
	};

}

