#pragma once
#include "RenderPass.h"
#include "Batch.h"

namespace Unique
{
	class ScenePass : public RenderPass
	{
	public:
		ScenePass();
		~ScenePass();

		virtual void Render(View* view);

	};

}

