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
		
		inline LLGL::Surface& GetSurface() const
		{
			return context->GetSurface();
		}

		inline LLGL::RenderSystem& GetRenderSystem() const
		{
			return *renderer;
		}

		inline LLGL::RenderContext& GetRenderContext() const
		{
			return *context;
		}

		// Render system
		UPtr<LLGL::RenderSystem>         renderer;

		// Main render context
		LLGL::RenderContext*                        context = nullptr;

		// Main command buffer
		LLGL::CommandBuffer*                        commands = nullptr;
	protected:
		bool debugger_ = false;
		bool vsync_ = false;
		int multiSampling_ = 4;
		UPtr<LLGL::RenderingProfiler>    profilerObj_;
		UPtr<LLGL::RenderingDebugger>    debuggerObj_;

	};

}

