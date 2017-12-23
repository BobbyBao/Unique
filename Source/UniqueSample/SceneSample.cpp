#include "UniquePCH.h"
#include "SceneSample.h"
#include "Scene/Scene.h"
#include "Graphics/Octree.h"
#include "Graphics/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/StaticModel.h"

//UNIQUE_IMPLEMENT_MAIN(Unique::SceneSample)

namespace Unique
{
	extern void Test();

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
		Test();

		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();

		StaticModel* model = scene_->CreateChild("Model")->CreateComponent<StaticModel>();
		model->SetModelAttr(ResourceRef::Create<Model>("Models/Kachujin/Kachujin.mdl"));

		SPtr<Material> mat(new Material());
		mat->SetShader(ResourceRef::Create<Shader>("Shaders/Basic.shader"));
		model->SetMaterial(mat);
		
		auto& renderer = GetSubsystem<Renderer>();
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);
	}

	void SceneSample::HandleShutdown(const struct Shutdown& eventData)
	{

	}

	void SceneSample::HandleUpdate(const struct Update& eventData)
	{

	}
}
