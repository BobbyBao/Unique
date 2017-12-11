#pragma once
#include "Application/Application.h"
#include "RefCntAutoPtr.h"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "BasicMath.h"

namespace Unique
{
	class Geometry;
	class Shader;
	class PipelineState;
	class ConstBuffer;

	class UniqueSample : public Application
	{
	public:
		UniqueSample();
		~UniqueSample();

	private:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnPreRender();
		virtual void OnPostRender();

		void CreateResource();
		void BuildSponge(int levelMax, bool aoEnabled);
		void SetShaderConstants(const float4x4& world, const float4x4& view, const float4x4& proj);

		::Quaternion m_SpongeRotation; // model rotation
		int m_SpongeLevel;           // number of recursions
		bool m_SpongeAO;             // apply ambient occlusion
		float3 m_LightDir;          // light direction vector
		float m_CamDistance;         // camera distance
		float m_BackgroundColor[4];  // background color
		bool m_Animate;              // enable animation
		float m_AnimationSpeed;      // animation speed

		Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pConstantBuffer;

		SPtr<ConstBuffer> constBuffer_;
		SPtr<Geometry> geometry_;
		SPtr<Shader> shader_;
		PipelineState* pipeline_;

		RefCntAutoPtr<IShader> vs_;
		RefCntAutoPtr<IShader> ps_;
		RefCntAutoPtr<IPipelineState> pipelineState_;
		RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;
		RefCntAutoPtr<IResourceMapping> resourceMapping_;
	};

}

