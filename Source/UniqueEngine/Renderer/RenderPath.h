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

		void Update(View* view);
		void Render(View* view);
	protected:
		Vector<SPtr<RenderPass>> renderPass_;
	};


}
