#pragma once
#include "Core/Semaphore.h"
#include <vector>
#include <functional>

namespace Unique
{
	using CommandQueue = std::vector<std::function<void()>>;

	class GraphicsContext
	{
	public:
		GraphicsContext();
		~GraphicsContext();

		static void BeginFrame();
		static void EndFrame();
		static void AddCommand(std::function<void()> cmd);

		static void BeginRender();
		static void EndRender();
		static void Stop();
		static int currentContext_;
		inline static int GetRenderContext() { return 1 - currentContext_; }
		
		static void FrameNoRenderWait();
	protected:
		static void ExecuteCommands(CommandQueue& cmds);

		static void MainSemPost();
		static bool MainSemWait(int _msecs = -1);
		static void SwapContext();
		static void RenderSemPost();
		static void RenderSemWait();

		static bool singleThreaded_;

		static Semaphore renderSem_;

		static Semaphore mainSem_;

		static long long waitSubmit_;

		static long long waitRender_;

		static CommandQueue comands_;;
	};


}
