#pragma once
#include "Application/Application.h"

namespace Unique
{
	class Scene;
	class Camera;

	class SceneSample : public Application
	{
	public:
		SceneSample(int argc, char* argv[]);
		~SceneSample();

	private:
		void HandleStartup(const struct Startup& eventData);
		void HandleShutdown(const struct Shutdown& eventData);
		void HandleGUI(const struct GUIEvent& eventData);
		void HandleUpdate(const struct Update& eventData);
		void UpdateCamera(float timeStep);

		SPtr<Scene> scene_;
		SPtr<Camera> camera_;
		Node* lightNode_;
		Node* node_;
		/// Camera yaw angle.
		float yaw_;
		/// Camera pitch angle.
		float pitch_;
		/// Flag to indicate whether touch input has been enabled.
		bool touchEnabled_;
		// Movement speed as world units per second
		float moveSpeed = 20.0f;
		// Mouse sensitivity as degrees per pixel
		float mouseSensitivity = 0.1f;
	};

}