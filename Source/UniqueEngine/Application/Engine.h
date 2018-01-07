#pragma once
#include "Core/Object.h"

namespace Unique
{
	class Engine : public Object, public Thread
	{
		uRTTI(Engine, Object)
	public:
		Engine();
		~Engine();


	private:
		virtual void ThreadFunction();

		void ApplyFrameLimit();

		/// Frame update timer.
		HiresTimer frameTimer_;
		/// Previous timesteps for smoothing.
		PODVector<float> lastTimeSteps_;
		/// Next frame timestep in seconds.
		float timeStep_;
		/// How many frames to average for the smoothed timestep.
		unsigned timeStepSmoothing_;
		/// Minimum frames per second.
		unsigned minFps_;
		/// Maximum frames per second.
		unsigned maxFps_;
		/// Maximum frames per second when the application does not have input focus.
		unsigned maxInactiveFps_;
		/// Pause when minimized flag.
		bool pauseMinimized_;
	};


}
