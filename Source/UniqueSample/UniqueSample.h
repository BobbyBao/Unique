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

	class UniqueSample : public Application
	{
	public:
		UniqueSample(Context* context);
		~UniqueSample();

		static SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled);
	private:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnPreRender();
		virtual void OnPostRender();

		void CreateResource();
		void SetShaderConstants(const Matrix4& world, const Matrix4& view, const Matrix4& proj);

		Quaternion m_SpongeRotation; // model rotation
		int m_SpongeLevel;           // number of recursions
		bool m_SpongeAO;             // apply ambient occlusion
		Vector3 m_LightDir;          // light direction vector
		float m_CamDistance;         // camera distance
		Color m_BackgroundColor;  // background color
		bool m_Animate;              // enable animation
		float m_AnimationSpeed;      // animation speed

		SPtr<ConstBuffer> constBuffer_;
		SPtr<Geometry> geometry_;
		SPtr<Shader> shader_;
		PipelineState* pipeline_;

		Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pConstantBuffer;
		RefCntAutoPtr<IPipelineState> pipelineState_;
		RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;

	};

}

