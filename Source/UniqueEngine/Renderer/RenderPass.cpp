#include "UniquePCH.h"
#include "RenderPass.h"

namespace Unique
{
	uEnum
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

	uEnum
	(
		RenderPassSortMode,
		"FRONTTOBACK",
		"BACKTOFRONT"
	);

	uObject(RenderPass)
	{
		uAttribute("Type", type_);
		uAttribute("SortMode", sortMode_);

	}

	RenderPass::RenderPass(RenderPassType type) : type_(type)
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

	ClearPass::ClearPass() : RenderPass(RenderPassType::CLEAR), clearColor_(0.2f, 0.2f, 0.2f)
	{
	}

	void ClearPass::Render(View* view)
	{
		auto& graphics = GetSubsystem<Graphics>();
		graphics.ClearRenderTarget(nullptr, clearColor_);
		graphics.ClearDepthStencil(nullptr, clearFlags_, clearDepth_, clearStencil_);
	}

}
