#include "UniquePCH.h"
#include "RenderPass.h"

namespace Unique
{
	uObject(RenderPass)
	{
	}

	RenderPass::RenderPass()
	{
	}

	RenderPass::~RenderPass()
	{
	}
	
	void RenderPass::Update(View* view)
	{
	}

	void RenderPass::Render(View* view)
	{
	}

	uObject(ClearPass)
	{
		uFactory()
	}

	void ClearPass::Render()
	{
		deviceContext->ClearRenderTarget(nullptr, clearColor_);
		deviceContext->ClearDepthStencil(nullptr, clearFlags_, clearDepth_, clearStencil_);
	}

}
