#pragma once
#include "Application/Application.h"
#include "RefCntAutoPtr.h"
#include "RenderDevice.h"
#include "DeviceContext.h"

namespace Unique
{
	class Geometry;
	class Shader;
	class PipelineState;
	class ConstBuffer;
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

		Quaternion m_SpongeRotation; // model rotation
		int m_SpongeLevel;           // number of recursions
		bool m_SpongeAO;             // apply ambient occlusion
		Vector3 m_LightDir;          // light direction vector
		float m_CamDistance;         // camera distance
		float m_BackgroundColor[4];  // background color
		bool m_Animate;              // enable animation
		float m_AnimationSpeed;      // animation speed

		SPtr<ConstBuffer> constBuffer_;
		SPtr<Geometry> geometry_;
		SPtr<Shader> shader_;
		PipelineState* pipeline_;

		Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pConstantBuffer;

		RefCntAutoPtr<IPipelineState> pipelineState_;
		RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;
		RefCntAutoPtr<IResourceMapping> resourceMapping_;
	};


}
