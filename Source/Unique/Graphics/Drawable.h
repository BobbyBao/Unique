#pragma once
#include "../Scene/Component.h"

namespace Unique
{

	class Drawable : public Component
	{
		uRTTI(Drawable, Component)
	public:
		Drawable();
		~Drawable();
	};


}
