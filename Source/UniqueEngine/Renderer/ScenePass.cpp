#include "UniquePCH.h"
#include "ScenePass.h"

namespace Unique
{
	uObject(ScenePass)
	{
		uFactory()
	}

	ScenePass::ScenePass()
	{
	}


	ScenePass::~ScenePass()
	{
	}

	void ScenePass::Render(View* view)
	{
		batchQueue_[Graphics::currentContext_].Draw(view, view->GetCamera());
	}
}
