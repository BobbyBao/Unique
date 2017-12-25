#include "UniquePCH.h"
#include "SceneSample.h"
#include "Scene/Scene.h"
#include "Graphics/Octree.h"
#include "Graphics/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/StaticModel.h"

UNIQUE_IMPLEMENT_MAIN(Unique::SceneSample)

namespace Unique
{
	extern void Test();

	struct ShaderConstants
	{
		Matrix4 WorldViewProjT;
		Matrix4 WorldNormT;
		Vector3 LightDir;
		float LightCoeff;
	};

	SceneSample::SceneSample(Context* context) : Application(context)
	{
		Subscribe(&SceneSample::HandleStartup);
		Subscribe(&SceneSample::HandleShutdown);
		Subscribe(&SceneSample::HandleUpdate);

	}

	SceneSample::~SceneSample()
	{
	}

	void SceneSample::HandleStartup(const struct Startup& eventData)
	{
		UNIQUE_UNUSED(eventData);

		Test();

		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();

		StaticModel* model = scene_->CreateChild("Model")->CreateComponent<StaticModel>();
		model->SetModelAttr(ResourceRef::Create<Model>("Models/Kachujin/Kachujin.mdl"));

		SPtr<Material> mat(new Material());
		mat->SetShader(ResourceRef::Create<Shader>("Shaders/Basic.shader"));
		model->SetMaterial(mat);

		constBuffer_ = new UniformBuffer(ShaderConstants(), USAGE_DYNAMIC, CPU_ACCESS_WRITE);
		
		auto& renderer = GetSubsystem<Renderer>();
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);

		auto& graphics = GetSubsystem<Graphics>();
		graphics.AddResource("Constants", constBuffer_);

	}

	void SceneSample::HandleShutdown(const struct Shutdown& eventData)
	{

	}

	void SceneSample::HandleUpdate(const struct Update& eventData)
	{
		auto& graphics = GetSubsystem<Graphics>();
		float aspectRatio = graphics.GetAspectRatio();

		Matrix4 proj = Matrix4::CreateProjection(M_PI / 4, aspectRatio, 0.1f, 100.0f, graphics.IsDirect3D());

		float dist = 2.0f;
		Vector3 camPosInv(dist * 0.3f, dist * 0.0f, dist * 2.0f);

		Matrix4 view = Matrix4::IDENTITY;
		view.SetTranslation(camPosInv);

		Matrix3x4 world = Matrix3x4::IDENTITY;// (Vector3::ZERO, spongeRotation_, Vector3::ONE);

		//MapHelper<ShaderConstants> MappedData(deviceContext, *constBuffer_, MAP_WRITE, MAP_FLAG_DISCARD);
		ShaderConstants *cst = (ShaderConstants *)constBuffer_->Lock();
		cst->WorldViewProjT = (proj* view *world);
		cst->WorldNormT = world.ToMatrix4();
		//cst->LightDir = (1.0f / m_LightDir.Length()) * m_LightDir;
		//cst->LightCoeff = 0.85f;
		constBuffer_->Unlock();
	}
}
