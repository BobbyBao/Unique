#include "UniquePCH.h"
#include "CharacterSample.h"
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

#define ENABLE_ANIM

//UNIQUE_IMPLEMENT_MAIN(Unique::CharacterSample)

namespace Unique
{
	extern void Test();
	extern SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled);

	CharacterSample::CharacterSample(int argc, char* argv[]) : Application(argc, argv)
	{
		Subscribe(&CharacterSample::HandleStartup);
		Subscribe(&CharacterSample::HandleShutdown);
		Subscribe(&CharacterSample::HandleUpdate);

		SetDeviceType(DeviceType::D3D11);
	//	SetDeviceType(DeviceType::OpenGL);
	}

	CharacterSample::~CharacterSample()
	{
	}

	void CharacterSample::HandleStartup(const struct Startup& eventData)
	{
		UNIQUE_UNUSED(eventData);

		auto& cache = GetSubsystem<ResourceCache>();
		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<DebugRenderer>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();

		node_ = scene_->CreateChild("Model");
		node_->SetRotation(Quaternion(0, 180, 0));

#ifdef ENABLE_ANIM
		AnimatedModel* model = node_->CreateComponent<AnimatedModel>();
#else
		StaticModel* model = node_->CreateComponent<StaticModel>();
#endif
		model->SetModelAttr(ResourceRef::Create<Model>("Models/Kachujin/Kachujin.mdl"));
  		model->SetMaterialsAttr(ResourceRefList::Create<Material>(
  		{ "Models/Kachujin/Materials/Kachujin.material" }));

#ifdef ENABLE_ANIM
		Animation* walkAnimation = cache.GetResource<Animation>("Models/Kachujin/Kachujin_Walk.ani");
		AnimationState* state = model->AddAnimationState(walkAnimation);
		// The state would fail to create (return null) if the animation was not found
		if (state)
		{
			// Enable full blending weight and looping
			state->SetWeight(1.0f);
			state->SetLooped(true);
			state->SetTime(Random(walkAnimation->GetLength()));
		}
#endif
		auto& renderer = GetSubsystem<Renderer>();
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);
		viewport->SetDrawDebug(true);
		camera_->GetNode()->SetPosition(Vector3(0, 1, -5));
	}

	void CharacterSample::HandleShutdown(const struct Shutdown& eventData)
	{

	}

	void CharacterSample::HandleGUI(const GUI& eventData)
	{

	}

	void CharacterSample::HandleUpdate(const struct Update& eventData)
	{
		auto& graphics = GetSubsystem<Graphics>();
		float aspectRatio = graphics.GetAspectRatio();
		
		UpdateCamera(eventData.timeStep_);

	}
	
	void CharacterSample::UpdateCamera(float timeStep)
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


#ifdef ENABLE_ANIM
		AnimatedModel* model = node_->GetComponent<AnimatedModel>(true);
		if (model && model->GetNumAnimationStates())
		{
			AnimationState* state = model->GetAnimationStates()[0];
			state->AddTime(timeStep);
		}
#endif
	}
}
