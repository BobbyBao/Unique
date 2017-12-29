#include "UniquePCH.h"
#include "SceneSample.h"
#include "Scene/Scene.h"
#include "Graphics/Octree.h"
#include "Graphics/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/StaticModel.h"
#include "Graphics/Model.h"
#include "Graphics/Geometry.h"
#include "Serialize/JsonSerializer.h"

UNIQUE_IMPLEMENT_MAIN(Unique::SceneSample)

namespace Unique
{
	extern void Test();
	extern SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled);

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

		//Test();

		auto& cache = GetSubsystem<ResourceCache>();

		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();

		node_ = scene_->CreateChild("Model");

		StaticModel* model = node_->CreateComponent<StaticModel>();
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
		mat->SetShader(ResourceRef::Create<Shader>("Shaders/Textured.shader"));

		{
			JsonSerializer jsonSer;
			jsonSer.Save("test_shader.json", SPtr<Shader>(mat->GetShader()));
		}

		Texture* tex = cache.GetResource<Texture>("Models/Kachujin/Textures/Kachujin_diffuse.png");
		mat->SetTexture("tDiffMap", tex);
		model->SetMaterial(mat);

		{
			JsonSerializer jsonSer;
			jsonSer.Save("test_material.json", mat);
		}

		auto& renderer = GetSubsystem<Renderer>();
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);

		camera_->GetNode()->SetPosition(Vector3(0, 1, -5));
	}

	void SceneSample::HandleShutdown(const struct Shutdown& eventData)
	{

	}

	void SceneSample::HandleUpdate(const struct Update& eventData)
	{
		auto& graphics = GetSubsystem<Graphics>();
		float aspectRatio = graphics.GetAspectRatio();

		node_->Rotate(Quaternion(0, 0.1f * eventData.timeStep_, 0));

	}
}
