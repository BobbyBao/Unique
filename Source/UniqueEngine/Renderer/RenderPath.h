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

		void AddPass(RenderPass* pass);
		void Update(View* view);
		void Render(View* view);

		inline Vector<SPtr<RenderPass>>& GetRenderPasses() { return renderPass_; }
	protected:
		Vector<SPtr<RenderPass>> renderPass_;
	};


}
