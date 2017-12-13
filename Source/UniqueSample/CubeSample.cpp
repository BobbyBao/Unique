#include "UniquePCH.h"
#include "CubeSample.h"
#include "Graphics/Shader.h"
#include "Graphics/PipelineState.h"
#include "MapHelper.h"
#include "Graphics/Geometry.h"
#include "Graphics/VertexBuffer.h"
#include "UniqueSample.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

//UNIQUE_IMPLEMENT_MAIN(Unique::CubeSample)

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
		Subscribe(&CubeSample::HandleUpdate);
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

		// Create vertex and index buffers
		geometry_ = UniqueSample::BuildSponge(m_SpongeLevel, m_SpongeAO);

		shader_ = cache.GetResource<Shader>("Shaders/Test.shader");
		pipeline_ = shader_->GetPipeline("Main", "");

		graphics.EndFrame();	

		m_pConstantBuffer = *constBuffer_;

		graphics.AddResource("Constants", m_pConstantBuffer, true);
	//	pipeline_->GetPipeline()->BindShaderResources(resourceMapping_, BIND_SHADER_RESOURCES_ALL_RESOLVED);

	}

	void CubeSample::HandleShutdown(const struct Shutdown& eventData)
	{
		UNIQUE_LOGINFO("Shutdown");
	}

	void CubeSample::HandleUpdate(const struct Update& eventData)
	{

	}

	void CubeSample::HandleRenderUpdate(const struct RenderUpdate& eventData)
	{
		Graphics& graphics = GetSubsystem<Graphics>();
		Vector<Batch>& batches = geometries_[graphics.currentContext_];
		batches.clear();
		Batch batch;
		batch.geometry_ = geometry_;
		batch.pipelineState_ = pipeline_;
		batches.emplace_back(batch);
	}
	
	void CubeSample::OnPreRender()
	{
		auto& graphics = GetSubsystem<Graphics>();
		// Clear the back buffer 
		deviceContext->ClearRenderTarget(nullptr, m_BackgroundColor);
		deviceContext->ClearDepthStencil(nullptr, CLEAR_DEPTH_FLAG, 1.f);

		// Set world/view/proj matrices and global shader constants
		float aspectRatio = graphics.GetAspectRatio();

		Matrix4 proj = Matrix4::CreateProjection(M_PI / 4, aspectRatio, 0.1f, 100.0f, graphics.IsDirect3D());

		float dist = m_CamDistance + 0.4f;
		Vector3 camPosInv(dist * 0.3f, dist * 0.0f, dist * 2.0f);

		Matrix4 view = Matrix4::IDENTITY;
		view.SetTranslation(camPosInv);

		Matrix4 world = Matrix4::IDENTITY;

		MapHelper<ShaderConstants> MappedData(deviceContext, m_pConstantBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
		ShaderConstants *cst = MappedData;
		cst->WorldViewProjT = (proj* view *world);
		cst->WorldNormT = world;
		cst->LightDir = (1.0f / m_LightDir.Length()) * m_LightDir;
		cst->LightCoeff = 0.85f;

		Vector<Batch>& batches = geometries_[graphics.GetRenderContext()];
		for (auto& batch : batches)
		{
			batch.geometry_->Draw(pipeline_);
		}
	}
}
