#pragma once
#include "Application/Application.h"
#include "RefCntAutoPtr.h"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "Renderer/View.h"

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
		CubeSample(Context* context);
		~CubeSample();

	protected:
		void HandleStartup(const struct Startup& eventData);
		void HandleShutdown(const struct Shutdown& eventData);
		void HandleUpdate(const struct Update& eventData);
		void HandleRenderUpdate(const struct RenderUpdate& eventData);

		virtual void OnPreRender();

		Quaternion spongeRotation_; // model rotation
		int m_SpongeLevel;           // number of recursions
		bool m_SpongeAO;             // apply ambient occlusion
		Vector3 m_LightDir;          // light direction vector
		float m_CamDistance;         // camera distance
		Color backgroundColor_;	// background color
		bool m_Animate = true;              // enable animation
		float m_AnimationSpeed;      // animation speed
		float yaw_ = 0.0f;

		SPtr<UniformBuffer> constBuffer_;
		SPtr<Geometry> geometry_;
		SPtr<Shader> shader_;
		PipelineState* pipeline_;
		
		Vector<Batch> geometries_[2];

	};


}
