#pragma once
#include "RenderPass.h"

namespace Unique
{
	class RenderPath : public Resource
	{
		uRTTI(RenderPath, Resource)
	public:
		RenderPath();
		~RenderPath();

		void AddPass(const RenderPass& pass);

		inline Vector<RenderPass>& GetRenderPasses() { return renderPass_; }
	protected:
		Vector<RenderPass> renderPass_;
	};


}
