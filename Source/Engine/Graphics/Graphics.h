#pragma once
#include "GraphicsDefs.h"
#include "../Core/Object.h"

namespace Unique
{
	class Graphics : public Object
	{
		uRTTI(Graphics, Object)
	public:
		Graphics();
		~Graphics();
		
		void SetDebug(bool val);

		void Initialize(const std::string& rendererModule, const LLGL::Size& size);

		bool BeginFrame();
		void EndFrame();
		
	protected:
		bool debugger_ = false;
		bool vsync_ = false;
		int multiSampling_ = 4;
		UPtr<LLGL::RenderingProfiler>    profilerObj_;
		UPtr<LLGL::RenderingDebugger>    debuggerObj_;
		Map<LLGL::SamplerDescriptor, LLGL::Sampler*> samplers_;
	};

}

