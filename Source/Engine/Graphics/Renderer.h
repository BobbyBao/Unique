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
		NoContext,
		Render,
		Timeout,
		Exiting,
		Count
	};

	class Renderer : public Object
	{
		uRTTI(Renderer, Object)
	public:
		Renderer();
		~Renderer();

		void AddCommand(std::function<void()> cmd);
		void PostCommand(std::function<void()> cmd);

		void Update();

		RenderFrameResult RenderFrame(int _msecs = -1);
	private:
		void apiSemPost();

		bool apiSemWait(int32_t _msecs = -1);

		void renderSemPost();

		void renderSemWait();

		void ExecuteCommands(CommandQueue& cmds);

		void flip();

		Semaphore m_renderSem;
		Semaphore m_apiSem;
		bool m_singleThreaded = false;

		Vector < std::function<void()>> preComands_;
		Vector < std::function<void()>> postComands_;

		Vector<SPtr<View>> views_;
	};

}

