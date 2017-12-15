#include "UniquePCH.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Octree.h"
#include "Core/CoreEvents.h"
#include "../Graphics/DebugRenderer.h"
#include "../Scene/Scene.h"

namespace Unique
{
	Renderer::Renderer() : graphics_(GetSubsystem<Graphics>())
	{
		Subscribe(&Renderer::HandleRenderUpdate);
		Subscribe(&Renderer::HandleEndFrame);
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::SetNumViewports(unsigned num)
	{
		viewports_.resize(num);
	}

	void Renderer::SetViewport(unsigned index, Viewport* viewport)
	{
		if (index >= viewports_.size())
			viewports_.resize(index + 1);

		viewports_[index] = viewport;
	}

	Viewport* Renderer::GetViewport(unsigned index) const
	{
		return index < viewports_.size() ? viewports_[index] : (Viewport*)0;
	}

	void Renderer::HandleRenderUpdate(const RenderUpdate& eventData)
	{
	//	URHO3D_PROFILE(UpdateViews);

		views_.clear();
		preparedViews_.clear();

		// Set up the frameinfo structure for this frame
		frame_.frameNumber_ = GetSubsystem<Time>().GetFrameNumber();
		frame_.timeStep_ = eventData.timeStep_;
		frame_.camera_ = 0;

		// Queue update of the main viewports. Use reverse order, as rendering order is also reverse
		// to render auxiliary views before dependent main views
		for (unsigned i = viewports_.size() - 1; i < viewports_.size(); --i)
			QueueViewport(0, viewports_[i]);

		// Update main viewports. This may queue further views
		unsigned numMainViewports = queuedViewports_.size();
		for (unsigned i = 0; i < numMainViewports; ++i)
			UpdateQueuedViewport(i);

		// Gather queued & autoupdated render surfaces
		//SendEvent(E_RENDERSURFACEUPDATE);

		// Update viewports that were added as result of the event above
		for (unsigned i = numMainViewports; i < queuedViewports_.size(); ++i)
			UpdateQueuedViewport(i);

		queuedViewports_.clear();
		resetViews_ = false;
		/*
		for (auto& view : views_)
		{
			view->Update();
		}
		*/
	}

	void Renderer::UpdateQueuedViewport(unsigned index)
	{
		WPtr<TextureView>& renderTarget = queuedViewports_[index].first;
		WPtr<Viewport>& viewport = queuedViewports_[index].second;

		// Null pointer means backbuffer view. Differentiate between that and an expired rendersurface
		if ((renderTarget.NotNull() && renderTarget.Expired()) || viewport.Expired())
			return;

		// (Re)allocate the view structure if necessary
		if (!viewport->GetView())
			viewport->AllocateView();

		View* view = viewport->GetView();
		assert(view);
		// Check if view can be defined successfully (has either valid scene, camera and octree, or no scene passes)
		if (!view->Define(renderTarget, viewport))
			return;

		views_.push_back(WPtr<View>(view));

		const IntRect& viewRect = viewport->GetRect();
		Scene* scene = viewport->GetScene();
		if (!scene)
			return;

		Octree* octree = scene->GetComponent<Octree>();

		// Update octree (perform early update for drawables which need that, and reinsert moved drawables.)
		// However, if the same scene is viewed from multiple cameras, update the octree only once
		if (!updatedOctrees_.count(octree))
		{
			frame_.camera_ = viewport->GetCamera();
			frame_.viewSize_ = viewRect.Size();
			if (frame_.viewSize_ == IntVector2::ZERO)
				frame_.viewSize_ = IntVector2(graphics_.GetWidth(), graphics_.GetHeight());
			octree->Update(frame_);
			updatedOctrees_.insert(octree);

			// Set also the view for the debug renderer already here, so that it can use culling
			/// \todo May result in incorrect debug geometry culling if the same scene is drawn from multiple viewports
			DebugRenderer* debug = scene->GetComponent<DebugRenderer>();
			if (debug && viewport->GetDrawDebug())
				debug->SetView(viewport->GetCamera());
		}

		// Update view. This may queue further views. View will send update begin/end events once its state is set
		//ResetShadowMapAllocations(); // Each view can reuse the same shadow maps
		view->Update(frame_);
	}

	void Renderer::HandleEndFrame(const EndFrame& args)
	{
		graphics_.Frame();
	}

	void Renderer::Begin()
	{
		graphics_.BeginRender();
	}

	void Renderer::Render()
	{
		for (auto& view : views_)
		{
			view->Render();
		}

	}

	void Renderer::End()
	{
		graphics_.EndRender();
	}

	void Renderer::Stop()
	{
		graphics_.Close();
	}
}