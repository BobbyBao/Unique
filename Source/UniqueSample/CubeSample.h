#pragma once
#include "Application/Application.h"
#include "RefCntAutoPtr.h"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "Graphics/View.h"

namespace Unique
{
	class Geometry;
	class Shader;
	class PipelineState;
	class UniformBuffer;
	class Matrix4;

	class CubeSample : public Application
	{
	public:
		CubeSample(int argc = 0, char* argv[] = nullptr);
		~CubeSample();

	protected:
		void HandleStartup(const struct Startup& eventData);
		void HandleShutdown(const struct Shutdown& eventData);
		void HandleUpdate(const struct Update& eventData);
		void HandleRenderUpdate(const struct RenderUpdate& eventData);
		
		Quaternion spongeRotation_; // model rotation
		int spongeLevel_;           // number of recursions
		bool spongeAO_;             // apply ambient occlusion
		Vector3 lightDir_;          // light direction vector
		float camDistance_;         // camera distance
		Color backgroundColor_;	// background color
		bool animate_ = true;              // enable animation
		float animationSpeed_;      // animation speed
		float yaw_;

		SPtr<UniformBuffer> constBuffer_;
		SPtr<Geometry> geometry_;
		SPtr<Shader> shader_;
		PipelineState* pipeline_;
		
		Vector<Batch> geometries_[2];

	};


}
