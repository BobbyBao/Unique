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

	void RenderPath::AddPass(const RenderPass& pass)
	{
		renderPass_.push_back(pass);
	}
	
}