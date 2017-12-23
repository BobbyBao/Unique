#pragma once
#include "RenderPass.h"
#include "Batch.h"

namespace Unique
{
	class ScenePass : public RenderPass
	{
		uRTTI(ScenePass, RenderPass)
	public:
		ScenePass();
		~ScenePass();

		virtual void Render(View* view);

	};

}

