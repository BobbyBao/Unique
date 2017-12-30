#pragma once

#include "Core/Event.h"

namespace Unique
{
	/// New screen mode set.
	uEvent(ScreenMode)
	{
		int width_;
		int height_;
		bool fullscreen_;
		bool borderless_;
		bool resizable_;
		bool highDPI_;
		int monitor_;
		int refreshRate_;
	};

	/// Window position changed.
	uEvent(WindowPos)
	{
		int x_;
		int y_;
	};
}
