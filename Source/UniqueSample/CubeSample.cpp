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
		uint AmbientColor;
	};

	struct ShaderConstants
	{
		Matrix4 WorldViewProjT;
		Matrix4 WorldNormT;
		Vector3 LightDir;
		float LightCoeff;
	};

	CubeSample::CubeSample(Context* context) : Application(context), backgroundColor_(0.0f, 0.0f, 0.5f, 1.0f)
	{
		m_SpongeLevel = 2;                      // number of recursions
		m_SpongeAO = true;                      // apply ambient occlusion
		m_LightDir.x_ = -0.5f;
		m_LightDir.y_ = -0.2f;
		m_LightDir.z_ = 1;
		m_CamDistance = 0.7f;                  // camera distance
		m_Animate = false;                     // enable animation
		m_AnimationSpeed = 0.2f;               // animation speed

		Subscribe(&CubeSample::HandleStartup);
		Subscribe(&CubeSample::HandleShutdown);
		Subscribe(&CubeSample::HandleUpdate);
		Subscribe(&CubeSample::HandleRenderUpdate);
	}


	CubeSample::~CubeSample()
	{
	}


	void CubeSample::HandleStartup(const struct Startup& eventData)
	{
		auto& cache = GetSubsystem<ResourceCache>();
		auto& graphics = GetSubsystem<Graphics>();

		constBuffer_ = new UniformBuffer();
		constBuffer_->Create(ShaderConstants(), USAGE_DYNAMIC, CPU_ACCESS_WRITE);

		// Create vertex and index buffers
		geometry_ = UniqueSample::BuildSponge(m_SpongeLevel, m_SpongeAO);

		shader_ = cache.GetResource<Shader>("Shaders/Test.shader");
		pipeline_ = shader_->GetPipeline("Main", "");

		graphics.Frame();	
		
		graphics.AddResource("Constants", constBuffer_);
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
		deviceContext->ClearRenderTarget(nullptr, backgroundColor_);
		deviceContext->ClearDepthStencil(nullptr, CLEAR_DEPTH_FLAG, 1.f);

		// Set world/view/proj matrices and global shader constants
		float aspectRatio = graphics.GetAspectRatio();

		Matrix4 proj = Matrix4::CreateProjection(M_PI / 4, aspectRatio, 0.1f, 100.0f, graphics.IsDirect3D());

		float dist = m_CamDistance + 0.4f;
		Vector3 camPosInv(dist * 0.3f, dist * 0.0f, dist * 2.0f);

		Matrix4 view = Matrix4::IDENTITY;
		view.SetTranslation(camPosInv);

		Matrix4 world = Matrix4::IDENTITY;


		Vector<Batch>& batches = geometries_[graphics.GetRenderContext()];
		for (auto& batch : batches)
		{
			MapHelper<ShaderConstants> MappedData(deviceContext, *constBuffer_, MAP_WRITE, MAP_FLAG_DISCARD);
			ShaderConstants *cst = MappedData;
			cst->WorldViewProjT = (proj* view *world);
			cst->WorldNormT = world;
			cst->LightDir = (1.0f / m_LightDir.Length()) * m_LightDir;
			cst->LightCoeff = 0.85f;

			batch.geometry_->Draw(pipeline_);
		}
	}
}
