#pragma once
#include "../Core/Object.h"
#include "../Core/Semaphore.h"
#include "../Core/Thread.h"
#include "../Graphics/View.h"

namespace Unique
{
	using CommandQueue = Vector<std::function<void()>>;
	
	enum class RenderFrameResult
	{
		Render,
		Timeout,
		Exiting
	};

	class Renderer : public Object
	{
		uRTTI(Renderer, Object)
	public:
		Renderer();
		~Renderer();

		void AddCommand(std::function<void()> cmd);
		void PostCommand(std::function<void()> cmd);

		//Execute in main thread
		void UpdateFrame();

		//Execute in render thread
		RenderFrameResult RenderFrame(int _msecs = -1);
	private:
		void Handle(StringID type, const EndFrame& args);

		void RenderUpdate();

		void FrameNoRenderWait();

		void MainSemPost();

		bool MainSemWait(int _msecs = -1);

		void SwapContext();

		void RenderSemPost();

		void RenderSemWait();

		void ExecuteCommands(CommandQueue& cmds);

		void Flip();

		Semaphore renderSem_;
		Semaphore mainSem_;
		bool singleThreaded_ = false;
		bool exit_ = false;

		Vector < std::function<void()>> preComands_;
		Vector < std::function<void()>> postComands_;

		Vector<SPtr<View>> views_;
		long long waitSubmit_ = 0;
		long long waitRender_ = 0;
	};

}

