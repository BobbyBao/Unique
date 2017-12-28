#include "UniquePCH.h"
#include "SceneSample.h"
#include "Scene/Scene.h"
#include "Graphics/Octree.h"
#include "Graphics/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/StaticModel.h"
#include "Graphics/Model.h"
#include "Graphics/Geometry.h"

UNIQUE_IMPLEMENT_MAIN(Unique::SceneSample)

namespace Unique
{
	extern void Test();
	extern SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled);

	struct ShaderConstants
	{
		Matrix4 WorldViewProjT;
		Matrix3x4 WorldNormT;
		Vector3 LightDir;
		float LightCoeff;
	};

	SceneSample::SceneSample()
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

		auto& cache = GetSubsystem<ResourceCache>();

		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();

		StaticModel* model = scene_->CreateChild("Model")->CreateComponent<StaticModel>();
		model->SetModelAttr(ResourceRef::Create<Model>("Models/Kachujin/Kachujin.mdl"));
		/*
		Model* m = new Model();
		m->SetNumGeometries(1);
		m->SetNumGeometryLodLevels(0, 1);
		SPtr<Geometry> geo = BuildSponge(2, true);
		m->SetGeometry(0, 0, geo);
		m->SetBoundingBox(BoundingBox(-10, 10));
		model->SetModel(m);*/

		SPtr<Material> mat(new Material());
		mat->SetShader(ResourceRef::Create<Shader>("Shaders/Basic.shader"));

		Texture* tex = cache.GetResource<Texture>("Models/Kachujin/Textures/Kachujin_diffuse.png");
		mat->SetTexture("sDiffMap", tex);
		model->SetMaterial(mat);

		constBuffer_ = new UniformBuffer(ShaderConstants(), USAGE_DYNAMIC, CPU_ACCESS_WRITE);
		
		auto& renderer = GetSubsystem<Renderer>();
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);

		auto& graphics = GetSubsystem<Graphics>();
		graphics.AddResource("Constants", constBuffer_);
		camera_->GetNode()->SetPosition(Vector3(0, 1, -5));
	}

	void SceneSample::HandleShutdown(const struct Shutdown& eventData)
	{

	}

	void SceneSample::HandleUpdate(const struct Update& eventData)
	{
		auto& graphics = GetSubsystem<Graphics>();
		float aspectRatio = graphics.GetAspectRatio();

		Matrix4 proj = camera_->GetGPUProjection();
		Matrix3x4 view = camera_->GetView();

		Matrix3x4 world(Vector3::ZERO, Quaternion::IDENTITY, Vector3::ONE);
		Vector3 lightDir(-0.5f, -0.2f, 1.0f);
		ShaderConstants *cst = (ShaderConstants *)constBuffer_->Lock();
		cst->WorldViewProjT = (proj* view *world);
		cst->WorldNormT = world.ToMatrix4();
		cst->LightDir = (1.0f / lightDir.Length()) * lightDir;
		cst->LightCoeff = 0.85f;
		constBuffer_->Unlock();
	}
}
