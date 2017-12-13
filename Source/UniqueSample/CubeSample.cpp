#include "UniquePCH.h"
#include "CubeSample.h"
#include "Graphics/Shader.h"
#include "Graphics/PipelineState.h"

#include "Graphics/Geometry.h"
#include "Graphics/VertexBuffer.h"
#include "UniqueSample.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

namespace Unique
{

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		unsigned int AmbientColor;
	};

	struct ShaderConstants
	{
		Matrix4 WorldViewProjT;
		Matrix4 WorldNormT;
		Vector3 LightDir;
		float LightCoeff;
	};

	CubeSample::CubeSample(Context* context) : Application(context)
	{
		Subscribe(&CubeSample::HandleStartup);
		Subscribe(&CubeSample::HandleShutdown);
		Subscribe(&CubeSample::HandleShutdown);
	}


	CubeSample::~CubeSample()
	{
	}


	void CubeSample::HandleStartup(const struct Startup& eventData)
	{
		auto& cache = GetSubsystem<ResourceCache>();
		auto& graphics = GetSubsystem<Graphics>();

		constBuffer_ = new ConstBuffer();
		constBuffer_->Create(ShaderConstants(), USAGE_DYNAMIC, CPU_ACCESS_WRITE);
		m_pConstantBuffer = constBuffer_->GetHandle();

		// Create vertex and index buffers
		geometry_ = UniqueSample::BuildSponge(m_SpongeLevel, m_SpongeAO);

		shader_ = cache.GetResource<Shader>("Shaders/Test.shader");

		graphics.EndFrame();

		pipeline_ = shader_->GetPipeline("Main", "");

		renderDevice->CreateResourceMapping(ResourceMappingDesc(), &resourceMapping_);
		resourceMapping_->AddResource("Constants", m_pConstantBuffer, true);
		pipeline_->GetPipeline()->BindShaderResources(resourceMapping_, BIND_SHADER_RESOURCES_ALL_RESOLVED);

	}

	void CubeSample::HandleShutdown(const struct Shutdown& eventData)
	{
		UNIQUE_LOGINFO("Shutdown");
	}

	void CubeSample::HandleUpdate(const struct Update& eventData)
	{

	}
}
