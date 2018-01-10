#include "UniquePCH.h"
#include "Engine.h"
#include "Core/CoreEvents.h"
#include "Core/WorkQueue.h"
#include "../Input/Input.h"
#include "ImGUI/ImGUI.h"

#include "Resource/Image.h"
#include "Graphics/Importers/ShaderImporter.h"
#include "Graphics/Importers/TextureImporter.h"
#include "Graphics/Importers/ModelImporter.h"

namespace Unique
{
	SPtr<Context> Engine::context_;
	Vector<String> Engine::argv_;
	bool Engine::quit_ = false;


	Engine::Engine() :
		resolution_(800, 600),
		deviceType_(DeviceType::D3D11),
		timeStep_(0.0f),
		timeStepSmoothing_(2),
		minFps_(10),
#if defined(IOS) || defined(TVOS) || defined(__ANDROID__) || defined(__arm__) || defined(__aarch64__)
		maxFps_(60),
		maxInactiveFps_(10),
		pauseMinimized_(true),
#else
		maxFps_(200),
		maxInactiveFps_(60),
		pauseMinimized_(false)
#endif
	{
		context_->RegisterSubsystem(this);
		context_->RegisterSubsystem<WorkQueue>();
		context_->RegisterSubsystem<Profiler>();
		context_->RegisterSubsystem<FileSystem>();
		context_->RegisterSubsystem<Log>("Unique.log");
		context_->RegisterSubsystem<Graphics>();
		context_->RegisterSubsystem<ResourceCache>();
		context_->RegisterSubsystem<Renderer>();
		context_->RegisterSubsystem<Input>();
		context_->RegisterSubsystem<GUI>();

	}

	Engine::~Engine()
	{
	}

	void Engine::Setup(int argc, char* argv[])
	{
		for (int i = 0; i < argc; i++)
		{
			argv_.push_back(argv[i]);
		}
	}

	void Engine::SetTitle(const String& title)
	{
		title_ = title;
	}

	void Engine::SetResolution(const IntVector2& res)
	{
		resolution_ = res;
	}

	void Engine::Initialize()
	{
		auto& cache = GetSubsystem<ResourceCache>();
		auto& graphics = GetSubsystem<Graphics>();

		cache.SetAutoReloadResources(true);
		cache.AddResourceDir("Assets");
		cache.AddResourceDir("CoreData");
		cache.AddResourceDir("Cache");

		cache.RegisterImporter(new ShaderImporter());
		cache.RegisterImporter(new ImageImporter());
		cache.RegisterImporter(new TextureImporter());
		cache.RegisterImporter(new ModelImporter());
		cache.RegisterImporter(new AnimationImporter());

		graphics.Initialize(resolution_, deviceType_);

		loadingDone_ = true;
	}


	void Engine::Start()
	{
		auto& renderer = GetSubsystem<Renderer>();
		auto& input = GetSubsystem<Input>();
		
		
		renderer.Render();

		Run();

		while (input.ProcessEvents() && !quit_)
		{
			renderer.Begin();
			renderer.Render();
			renderer.End();
		}

#ifdef __EMSCRIPTEN__

		emscripten_cancel_main_loop();

#endif
		renderer.Stop();

		Stop();

	}

	void Engine::ThreadFunction()
	{
		// Set the main thread ID (assuming the Context is created in it)
		Thread::SetMainThread();

		auto& timer = GetSubsystem<Time>();

		SendEvent(Startup());

		frameTimer_.Reset();

		while (shouldRun_)
		{
			timer.BeginFrame(timeStep_);

			{
				UNIQUE_PROFILE(Update);

				Unique::Update eventData;
				eventData.timeStep_ = timeStep_;

				SendEvent(eventData);
				// Logic post-update event
				SendEvent((const PostUpdate&)eventData);
				// Rendering update event
				SendEvent((const RenderUpdate&)eventData);
				// Post-render update event
				SendEvent((const PostRenderUpdate&)eventData);
			}

			ApplyFrameLimit();
			timer.EndFrame();
		}

		SendEvent(Shutdown());

	}

	void Engine::ApplyFrameLimit()
	{
		unsigned maxFps = maxFps_;

		long long elapsed = 0;

#ifndef __EMSCRIPTEN__
		// Perform waiting loop if maximum FPS set
#if !defined(IOS) && !defined(TVOS)
		if (maxFps)
#else
		// If on iOS/tvOS and target framerate is 60 or above, just let the animation callback handle frame timing
		// instead of waiting ourselves
		if (maxFps < 60)
#endif
		{
			UNIQUE_PROFILE(ApplyFrameLimit);

			long long targetMax = 1000000LL / maxFps;

			for (;;)
			{
				elapsed = frameTimer_.GetUSec(false);
				if (elapsed >= targetMax)
					break;

				// Sleep if 1 ms or more off the frame limiting goal
				if (targetMax - elapsed >= 1000LL)
				{
					unsigned sleepTime = (unsigned)((targetMax - elapsed) / 1000LL);
					Time::Sleep(sleepTime);
				}
			}
		}
#endif

		elapsed = frameTimer_.GetUSec(true);

		// If FPS lower than minimum, clamp elapsed time
		if (minFps_)
		{
			long long targetMin = 1000000LL / minFps_;
			if (elapsed > targetMin)
				elapsed = targetMin;
		}

		// Perform timestep smoothing
		timeStep_ = 0.0f;
		lastTimeSteps_.push_back(elapsed / 1000000.0f);
		if (lastTimeSteps_.size() > timeStepSmoothing_)
		{
			// If the smoothing configuration was changed, ensure correct amount of samples
			lastTimeSteps_.erase(lastTimeSteps_.begin(), lastTimeSteps_.begin() + lastTimeSteps_.size() - timeStepSmoothing_);
			for (unsigned i = 0; i < lastTimeSteps_.size(); ++i)
				timeStep_ += lastTimeSteps_[i];
			timeStep_ /= lastTimeSteps_.size();
		}
		else
			timeStep_ = lastTimeSteps_.back();
	}
}
