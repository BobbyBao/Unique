#pragma once
#include "RenderPass.h"
#include "Batch.h"

namespace Unique
{
	class ScenePass : RenderPass
	{
		uRTTI(ScenePass, RenderPass)
	public:
		ScenePass();
		~ScenePass();

		virtual void Render();
	protected:
		BatchQueue batchQueue_[2];
	};

}

