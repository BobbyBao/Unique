#include "UniquePCH.h"
#include "ScenePass.h"

namespace Unique
{
	ScenePass::ScenePass() : RenderPass(RenderPassType::SCENEPASS)
	{
	}


	ScenePass::~ScenePass()
	{
	}

	void ScenePass::Render(View* view)
	{
		auto& batchQueues = RenderContext(view->batchQueues_);
		BatchQueue& queue = batchQueues[passIndex_];
		if (!queue.IsEmpty())
		{
			queue.Draw(view, view->GetCamera());
		}
	}
}
