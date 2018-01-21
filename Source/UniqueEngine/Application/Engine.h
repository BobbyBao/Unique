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

		void SetDeviceType(DeviceType deviceType) { deviceType_ = deviceType; }
		void SetTitle(const String& title);
		void SetResolution(const IntVector2& res);
		void Initialize();
		void Start();
		
		/// Set minimum frames per second. If FPS goes lower than this, time will appear to slow down.
		void SetMinFps(int fps);
		/// Set maximum frames per second. The engine will sleep if FPS is higher than this.
		void SetMaxFps(int fps);

		/// Get timestep of the next frame. Updated by ApplyFrameLimit().
		float GetNextTimeStep() const { return timeStep_; }
		/// Return the minimum frames per second.
		int GetMinFps() const { return minFps_; }
		/// Return the maximum frames per second.
		int GetMaxFps() const { return maxFps_; }

		static void Setup(int argc, char* argv[]);

		static SPtr<Context> context_;
		static Vector<String> argv_;
		static bool	quit_;

	private:
		virtual void ThreadFunction();

		void ApplyFrameLimit();

		DeviceType	deviceType_;
		String		title_;
		IntVector2	resolution_;
		bool        loadingDone_ = false;
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
