#pragma once

#include "RenderPass.h"
#include "Batch.h"

namespace Unique
{
	class LightPass : public RenderPass
	{
		uRTTI(LightPass, RenderPass)
	public:
		LightPass();
		~LightPass();
	};

}
