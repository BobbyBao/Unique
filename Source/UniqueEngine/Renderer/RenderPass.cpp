#include "UniquePCH.h"
#include "RenderPass.h"

namespace Unique
{
	uEnumTraits
	(
		RenderPassType,
		"NONE",
		"CLEAR",
		"SCENEPASS",
		"QUAD",
		"FORWARDLIGHTS",
		"LIGHTVOLUMES",
		"RENDERUI",
		"SENDEVENT"
	);

	uEnumTraits
	(
		RenderPassSortMode,
		"FRONTTOBACK",
		"BACKTOFRONT"
	);

	uObject(RenderPass)
	{
		uAttribute("Type", type_);
		uAttribute("Type", type_);

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

	void ClearPass::Render(View* view)
	{
		deviceContext->ClearRenderTarget(nullptr, clearColor_);
		deviceContext->ClearDepthStencil(nullptr, clearFlags_, clearDepth_, clearStencil_);
	}

}
