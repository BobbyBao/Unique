#pragma once
#include "Application/Application.h"

namespace Unique
{
	class Scene;
	class Camera;

	class SceneSample : public Application
	{
	public:
		SceneSample();
		~SceneSample();

	private:
		void HandleStartup(const struct Startup& eventData);
		void HandleShutdown(const struct Shutdown& eventData);
		void HandleUpdate(const struct Update& eventData);
		void UpdateCamera(float timeStep);

		SPtr<Scene> scene_;
		SPtr<Camera> camera_;
		Node* node_;

		IntVector3 mouseNow_;
		IntVector3 mouseLast_;

		/// Camera yaw angle.
		float yaw_;
		/// Camera pitch angle.
		float pitch_;
		/// Flag to indicate whether touch input has been enabled.
		bool touchEnabled_;
		/// Mouse mode option to use in the sample.
		//MouseMode useMouseMode_;
	};

}