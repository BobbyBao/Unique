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

		SPtr<Scene> scene_;
		SPtr<Camera> camera_;
		SPtr<UniformBuffer> constBuffer_;
		Node* node_;
	};

}