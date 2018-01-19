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

	uStruct(RenderPass)
	{
		uAttribute("Type", type_);
		uAttribute("SortMode", sortMode_);

	}

	RenderPass::RenderPass(RenderPassType type) : type_(type), clearColor_(0.2f, 0.2f, 0.2f)
	{
	}

	RenderPass::~RenderPass()
	{
	}
	
	ClearPass::ClearPass() : RenderPass(RenderPassType::CLEAR)
	{
	}

	void ClearPass::Render(View* view)
	{
		auto& graphics = GetSubsystem<Graphics>();
		graphics.ClearRenderTarget(nullptr, clearColor_);
		graphics.ClearDepthStencil(nullptr, clearFlags_, clearDepth_, clearStencil_);
	}

}
