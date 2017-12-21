#pragma once

namespace Unique
{
	class RenderPass;

	class RenderPath : public Object
	{
		uRTTI(RenderPath, Object)
	public:
		RenderPath();
		~RenderPath();
	protected:
		Vector<SPtr<RenderPass>> renderPass_;
	};


}
