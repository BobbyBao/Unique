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
		auto& cache = GetSubsystem<ResourceCache>();
		scene_ = new Scene();
		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<DebugRenderer>();
		camera_ = scene_->CreateChild("Camera")->CreateComponent<Camera>();
		camera_->SetFarClip(2000);

		lightNode_ = scene_->CreateChild("DirectionalLight");
		lightNode_->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
		Light* light = lightNode_->CreateComponent<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);

		node_ = scene_->CreateChild("Model");
		node_->SetRotation(Quaternion(0, 180, 0));

		StaticModel* model = node_->CreateComponent<StaticModel>();
		model->SetModelAttr(ResourceRef::Create<Model>(
			"Models/crytek-sponza/sponza.obj"
			//"Models/Sponza/sponza.obj"
			//"Models/map-bump.obj"
			));
//   		model->SetMaterialsAttr(ResourceRefList::Create<Material>(
//   		{ "Models/map-bump.material" }));

		auto& renderer = GetSubsystem<Renderer>();
		Zone* zone = renderer.GetDefaultZone();
		zone->SetAmbientColor(Color(0.11f, 0.27f, 0.54f));
		zone->SetFogColor(Color(0.4f, 0.5f, 1.0f));
		Viewport* viewport = new Viewport(scene_, camera_);
		renderer.SetViewport(0, viewport);
		viewport->SetDrawDebug(true);
		camera_->GetNode()->SetPosition(Vector3(0, 1, -30));
	}

	void SceneSample::HandleShutdown(const struct Shutdown& eventData)
	{
	}

	void SceneSample::HandleGUI(const GUIEvent& eventData)
	{
		auto& graphics = GetSubsystem<Graphics>();
		auto& renderer = GetSubsystem<Renderer>();
		nk_context* ctx = nk_ctx();
		if (nk_begin(ctx, "Static Scene", nk_rect((float)graphics.GetWidth() - 220, 20, 200, 500),
			NK_WINDOW_BORDER | /*NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |*/
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
		{
			Zone* zone = renderer.GetDefaultZone();
			Engine& engine = GetSubsystem<Engine>();

			nk_layout_row_dynamic(ctx, 20, 2);
			
			nk_label(ctx, "FPS:", NK_TEXT_LEFT);
			nk_label(ctx, String(1 / engine.GetNextTimeStep()), NK_TEXT_LEFT);
			nk_label(ctx, "Render Wait:", NK_TEXT_LEFT);
			nk_label(ctx, String(Graphics::GetRenderWait()), NK_TEXT_LEFT);
			nk_label(ctx, "Update Wait:", NK_TEXT_LEFT);
			nk_label(ctx, String(Graphics::GetUpdateWait()), NK_TEXT_LEFT);
			nk_label(ctx, "Batch Count:", NK_TEXT_LEFT);
			nk_label(ctx, String(Graphics::GetBatchCount()), NK_TEXT_LEFT);
			nk_label(ctx, "Tri Count:", NK_TEXT_LEFT);
			nk_label(ctx, String(Graphics::GetTriCount()), NK_TEXT_LEFT);
#if false
			Color ambient = GUI::Property("Ambient Color: ", zone->GetAmbientColor(), false);
			if (ambient != zone->GetAmbientColor())
			{
				zone->SetAmbientColor(ambient);
			}

			Color fogColor = GUI::Property("Fog Color: ", zone->GetFogColor(), false);
			if (fogColor != zone->GetAmbientColor())
			{
				zone->SetFogColor(fogColor);
			}

			float fogStart = nk_propertyf(ctx, "Fog Start", 0, zone->GetFogStart(), 3000, 1, 0.5f);
			if (fogStart != zone->GetFogStart())
			{
				zone->SetFogStart(fogStart);
			}

			float fogEnd = nk_propertyf(ctx, "Fog End", 0, zone->GetFogEnd(), 3000, 1, 0.5f);
			if (fogEnd != zone->GetFogEnd())
			{
				zone->SetFogEnd(fogEnd);
			}

			Vector3 dir = GUI::Property("Light dir: ", Vector3(-1.0f, -1.0f, -1.0f), lightNode_->GetDirection(), Vector3(1.0f, 1.0f, 1.0f));
			if (dir != lightNode_->GetDirection())
			{
				lightNode_->SetDirection(dir);
			}

			nk_layout_row_dynamic(ctx, 25, 1);

			nk_label(ctx, "Camera:", NK_TEXT_ALIGN_LEFT);

			float farClip = nk_propertyf(ctx, "Camera Far Clip", 100, camera_->GetFarClip(), 3000, 1, 0.5f);
			if (farClip != camera_->GetFarClip())
			{
				camera_->SetFarClip(farClip);
			}

			nk_property_float(ctx, "Move Speed", -100.0f, &moveSpeed, 100.0f, 0.1f, 0.05f);
			nk_property_float(ctx, "Mouse Sensitivity", -100.0f, &mouseSensitivity, 100.0f, 0.1f, 0.05f);
#endif
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


		if (input.GetMouseButtonDown(MOUSEB_RIGHT))
		{
			// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
			IntVector2 mouseMove = input.GetMouseMove();
			yaw_ += mouseSensitivity * mouseMove.x_;
			pitch_ += mouseSensitivity * mouseMove.y_;
			pitch_ = Clamp(pitch_, -90.0f, 90.0f);

			// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
			camera_->GetNode()->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
		}

		// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
		// Use the Translate() function (default local space) to move relative to the node's orientation.
		if (input.GetKeyDown(KEY_W))
			camera_->GetNode()->Translate(Vector3::FORWARD * moveSpeed * timeStep);
		if (input.GetKeyDown(KEY_S))
			camera_->GetNode()->Translate(Vector3::BACK * moveSpeed * timeStep);
		if (input.GetKeyDown(KEY_A))
			camera_->GetNode()->Translate(Vector3::LEFT * moveSpeed * timeStep);
		if (input.GetKeyDown(KEY_D))
			camera_->GetNode()->Translate(Vector3::RIGHT * moveSpeed * timeStep);

	}
}
