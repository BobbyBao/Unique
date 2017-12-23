#include "UniquePCH.h"
#include "RenderPath.h"
#include "RenderPass.h"

namespace Unique
{
	uObject(RenderPath)
	{
		uFactory("Renderer")

	}

	RenderPath::RenderPath()
	{
	}

	RenderPath::~RenderPath()
	{
	}

	void RenderPath::AddPass(RenderPass* pass)
	{
		renderPass_.push_back(SPtr<RenderPass>(pass));
	}
	
	void RenderPath::Update(View* view)
	{
		for (auto& pass : renderPass_)
		{
			pass->Update(view);
		}
	}
	
	void RenderPath::Render(View* view)
	{
		for (auto& pass : renderPass_)
		{
			pass->Render(view);
		}
	}
}