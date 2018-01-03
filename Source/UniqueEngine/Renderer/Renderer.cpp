#include "UniquePCH.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Octree.h"
#include "Graphics/Geometry.h"
#include "Core/CoreEvents.h"
#include "../Graphics/DebugRenderer.h"
#include "../Scene/Scene.h"
#include "RenderPath.h"
#include "ScenePass.h"

namespace Unique
{
	static const int MAX_EXTRA_INSTANCING_BUFFER_ELEMENTS = 4;

	inline PODVector<VertexElement> CreateInstancingBufferElements(unsigned numExtraElements)
	{
		static const unsigned NUM_INSTANCEMATRIX_ELEMENTS = 3;
		static const unsigned FIRST_UNUSED_TEXCOORD = 4;

		PODVector<VertexElement> elements;
		for (unsigned i = 0; i < NUM_INSTANCEMATRIX_ELEMENTS + numExtraElements; ++i)
			elements.push_back(VertexElement(TYPE_VECTOR4, SEM_TEXCOORD, FIRST_UNUSED_TEXCOORD + i, true));
		return elements;
	}

	Renderer::Renderer() : graphics_(GetSubsystem<Graphics>()), numExtraInstancingBufferElements_(3)
	{
		Initialize();
	}

	Renderer::~Renderer()
	{
	}

	/// Initialize when screen mode initially set.
	void Renderer::Initialize()
	{
		defaultRenderPath_ = new RenderPath();
		defaultRenderPath_->AddPass(new ClearPass());
		defaultRenderPath_->AddPass(new ScenePass());

		Subscribe(&Renderer::HandleRenderUpdate);
		Subscribe(&Renderer::HandleEndFrame);
	}

	void Renderer::LoadShaders()
	{

	}

	void Renderer::SetNumExtraInstancingBufferElements(int elements)
	{
		if (numExtraInstancingBufferElements_ != elements)
		{
			numExtraInstancingBufferElements_ = Clamp(elements, 0, MAX_EXTRA_INSTANCING_BUFFER_ELEMENTS);
			CreateInstancingBuffer();
		}
	}

	void Renderer::SetMinInstances(int instances)
	{
		minInstances_ = Max(instances, 1);
	}

	void Renderer::SetMaxSortedInstances(int instances)
	{
		maxSortedInstances_ = Max(instances, 0);
	}

	void Renderer::SetNumViewports(unsigned num)
	{
		assert(Thread::IsMainThread());
		viewports_.resize(num);
	}

	void Renderer::SetViewport(unsigned index, Viewport* viewport)
	{
		assert(Thread::IsMainThread());

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
		UNIQUE_PROFILE(UpdateViews);

		auto& views = MainContext(views_);
		views.clear();
		preparedViews_.clear();

		// Set up the frameinfo structure for this frame
		frame_.frameNumber_ = GetSubsystem<Time>().GetFrameNumber();
		frame_.timeStep_ = eventData.timeStep_;
		frame_.camera_ = 0;
// 		numShadowCameras_ = 0;
// 		numOcclusionBuffers_ = 0;
		updatedOctrees_.clear();

		// Queue update of the main viewports. Use reverse order, as rendering order is also reverse
		// to render auxiliary views before dependent main views
		for (size_t i = viewports_.size() - 1; i < viewports_.size(); --i)
			QueueViewport(0, viewports_[i]);

		// Update main viewports. This may queue further views
		unsigned numMainViewports = (unsigned)queuedViewports_.size();
		for (unsigned i = 0; i < numMainViewports; ++i)
			UpdateQueuedViewport(i);

		// Gather queued & autoupdated render surfaces
		//SendEvent(E_RENDERSURFACEUPDATE);

		// Update viewports that were added as result of the event above
		for (unsigned i = numMainViewports; i < queuedViewports_.size(); ++i)
			UpdateQueuedViewport(i);

		queuedViewports_.clear();
		//resetViews_ = false;
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
		auto& views = RenderContext(views_);

		for (auto view : views)
		{
			view->Render();
		}

	}

	void Renderer::DrawDebugGeometry(bool depthTest)
	{
	//	UNIQUE_PROFILE(RendererDrawDebug);

		/// \todo Because debug geometry is per-scene, if two cameras show views of the same area, occlusion is not shown correctly
		HashSet<Drawable*> processedGeometries;
		HashSet<Light*> processedLights;

		auto& views = MainContext(views_);

		for (unsigned i = 0; i < views.size(); ++i)
		{
			View* view = views[i];
			if (!view || !view->GetDrawDebug())
				continue;
			Octree* octree = view->GetOctree();
			if (!octree)
				continue;
			DebugRenderer* debug = octree->GetComponent<DebugRenderer>();
			if (!debug || !debug->IsEnabledEffective())
				continue;

			// Process geometries / lights only once
			const PODVector<Drawable*>& geometries = view->GetGeometries();
			const PODVector<Light*>& lights = view->GetLights();

			for (size_t i = 0; i < geometries.size(); ++i)
			{
				if (!processedGeometries.count(geometries[i]))
				{
					geometries[i]->DrawDebugGeometry(debug, depthTest);
					processedGeometries.insert(geometries[i]);
				}
			}

			for (size_t i = 0; i < lights.size(); ++i)
			{
				if (!processedLights.count(lights[i]))
				{
					lights[i]->DrawDebugGeometry(debug, depthTest);
					processedLights.insert(lights[i]);
				}
			}
		}

		for (auto view : views)
		{
			view->DrawDebug();
		}
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

		auto& views = MainContext(views_);
		views.push_back(WPtr<View>(view));

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

	void Renderer::QueueViewport(TextureView* renderTarget, Viewport* viewport)
	{
		if (viewport)
		{
			Pair<WPtr<TextureView>, WPtr<Viewport> > newView =
				std::make_pair(WPtr<TextureView>(renderTarget), WPtr<Viewport>(viewport));

			// Prevent double add of the same rendertarget/viewport combination
			if (!Contains(queuedViewports_, newView))
				queuedViewports_.push_back(newView);
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

	void Renderer::SetBatchShaders(Batch& batch, Shader* tech, bool allowShadows, const BatchQueue& queue)
	{
		batch.pipelineState_ = batch.pass_->GetPipeline(tech, "");
	}

	void Renderer::CreateInstancingBuffer()
	{
		instancingBuffer_ = new VertexBuffer();
		const PODVector<VertexElement> instancingBufferElements = CreateInstancingBufferElements(numExtraInstancingBufferElements_);
		if (!instancingBuffer_->SetSize(INSTANCING_BUFFER_DEFAULT_SIZE, instancingBufferElements, true))
		{
			instancingBuffer_.Reset();
		}
	}


	bool Renderer::ResizeInstancingBuffer(unsigned numInstances)
	{
		if (!instancingBuffer_)
			return false;

		unsigned oldSize = instancingBuffer_->GetCount();
		if (numInstances <= oldSize)
			return true;

		unsigned newSize = INSTANCING_BUFFER_DEFAULT_SIZE;
		while (newSize < numInstances)
			newSize <<= 1;
		
		const PODVector<VertexElement> instancingBufferElements = CreateInstancingBufferElements(numExtraInstancingBufferElements_);
		if (!instancingBuffer_->SetSize(newSize, instancingBufferElements, true))
		{
			UNIQUE_LOGERROR("Failed to resize instancing buffer to " + String(newSize));
			// If failed, try to restore the old size
			instancingBuffer_->SetSize(oldSize, instancingBufferElements, true);
			return false;
		}

		UNIQUE_LOGDEBUG("Resized instancing buffer to " + String(newSize));
		return true;
	}
}