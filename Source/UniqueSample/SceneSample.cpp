#include "UniquePCH.h"
#include "SceneSample.h"
#include "Scene/Scene.h"
#include "Graphics/Octree.h"
#include "Graphics/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/StaticModel.h"
#include "Animation/AnimatedModel.h"
#include "Animation/Animation.h"
#include "Animation/AnimationState.h"

#include "Graphics/Model.h"
#include "Graphics/Geometry.h"
#include "Graphics/DebugRenderer.h"
#include "Serialize/JsonSerializer.h"
#include "Input/Input.h"
#include "ImGUI/ImGUI.h"

#define ENABLE_ANIM

UNIQUE_IMPLEMENT_MAIN(Unique::SceneSample)

namespace Unique
{
	extern void Test();
	extern SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled);

	SceneSample::SceneSample(int argc, char* argv[]) : Application(argc, argv)
	{
		Subscribe(&SceneSample::HandleStartup);
		Subscribe(&SceneSample::HandleShutdown);
		Subscribe(&SceneSample::HandleUpdate);
		Subscribe(&SceneSample::HandleGUI);

	//	SetDeviceType(DeviceType::OpenGL);
	}

	SceneSample::~SceneSample()
	{
	}

	void SceneSample::HandleStartup(const struct Startup& eventData)
	{
		UNIQUE_UNUSED(eventData);

		auto& cache = GetSubsystem<ResourceCache>();
		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<DebugRenderer>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();

		node_ = scene_->CreateChild("Model");
		node_->SetRotation(Quaternion(0, 180, 0));

		StaticModel* model = node_->CreateComponent<StaticModel>();
		model->SetModelAttr(ResourceRef::Create<Model>(
			"Models/Sponza/sponza.obj"
			//"Models/Corridor2/Map.obj"
			//"Models/map-bump.obj"
			));
  		model->SetMaterialsAttr(ResourceRefList::Create<Material>(
  		{ "Models/map-bump.material" }));

		auto& renderer = GetSubsystem<Renderer>();
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);
		viewport->SetDrawDebug(true);
		camera_->GetNode()->SetPosition(Vector3(0, 1, -30));
	}

	void SceneSample::HandleShutdown(const struct Shutdown& eventData)
	{

	}

	Vector3 test;
	void SceneSample::HandleGUI(const GUIEvent& eventData)
	{		
		if (nk_begin(nk_ctx(), "Demo", nk_rect(50, 50, 230, 250),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
		{
			nk_layout_row_static(nk_ctx(), 30, 80, 1);
			nk_property_vector3(nk_ctx(), "pos:", test);
		}
		nk_end(nk_ctx());
	}

	void SceneSample::HandleUpdate(const struct Update& eventData)
	{
		auto& graphics = GetSubsystem<Graphics>();
		float aspectRatio = graphics.GetAspectRatio();
		
		UpdateCamera(eventData.timeStep_);

	}
	
	void SceneSample::UpdateCamera(float timeStep)
	{
		auto& input = GetSubsystem<Input>();

		// Movement speed as world units per second
		const float MOVE_SPEED = 20.0f;
		// Mouse sensitivity as degrees per pixel
		const float MOUSE_SENSITIVITY = 0.1f;

		if (input.GetMouseButtonDown(MOUSEB_RIGHT))
		{
			// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
			IntVector2 mouseMove = input.GetMouseMove();
			yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
			pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
			pitch_ = Clamp(pitch_, -90.0f, 90.0f);

			// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
			camera_->GetNode()->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
		}

		// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
		// Use the Translate() function (default local space) to move relative to the node's orientation.
		if (input.GetKeyDown(KEY_W))
			camera_->GetNode()->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
		if (input.GetKeyDown(KEY_S))
			camera_->GetNode()->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
		if (input.GetKeyDown(KEY_A))
			camera_->GetNode()->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
		if (input.GetKeyDown(KEY_D))
			camera_->GetNode()->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

	}
}
