#pragma once
#include "GraphicsDefs.h"
#include "../Core/Object.h"
#include "../Core/Semaphore.h"

namespace Unique
{
	using CommandQueue = Vector<std::function<void()>>;

	enum class RenderFrameResult
	{
		Render,
		Timeout,
		Exiting
	};

	class Graphics : public Object
	{
		uRTTI(Graphics, Object)
	public:
		Graphics();
		~Graphics();
		
		void SetDebug(bool val);

		void Initialize(const std::string& rendererModule, const LLGL::Size& size);

		void AddCommand(std::function<void()> cmd);
		void PostCommand(std::function<void()> cmd);

		void BeginFrame();
		void EndFrame();

		//Execute in render thread
		RenderFrameResult RenderFrame(int _msecs = -1);
	protected:

		void FrameNoRenderWait();

		void MainSemPost();

		bool MainSemWait(int _msecs = -1);

		void SwapContext();

		void RenderSemPost();

		void RenderSemWait();

		void ExecuteCommands(CommandQueue& cmds);

		void Flip();

		bool debugger_ = false;
		bool vsync_ = false;
		int multiSampling_ = 4;
		UPtr<LLGL::RenderingProfiler>    profilerObj_;
		UPtr<LLGL::RenderingDebugger>    debuggerObj_;
		Map<LLGL::SamplerDescriptor, LLGL::Sampler*> samplers_;


		Semaphore renderSem_;
		Semaphore mainSem_;
		bool singleThreaded_ = false;
		bool exit_ = false;

		Vector < std::function<void()>> preComands_;
		Vector < std::function<void()>> postComands_;

		long long waitSubmit_ = 0;
		long long waitRender_ = 0;
	};

}

