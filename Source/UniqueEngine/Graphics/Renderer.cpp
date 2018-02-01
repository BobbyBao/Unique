#include "UniquePCH.h"
#include "Renderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Octree.h"
#include "Graphics/Geometry.h"
#include "Core/CoreEvents.h"
#include "../Graphics/DebugRenderer.h"
#include "../Scene/Scene.h"
#include "RenderPath.h"
#include "Model.h"

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

	Renderer::Renderer() 
		: graphics_(GetSubsystem<Graphics>()), numExtraInstancingBufferElements_(0), drawDebug_(true)
	{
		Initialize();

		scissorRect_[0].reserve(64);
		scissorRect_[0].emplace_back(0, 0, 0, 0);
		scissorRect_[1].reserve(64);
		scissorRect_[1].emplace_back(0, 0, 0, 0);

	}

	Renderer::~Renderer()
	{
	}

	/// Initialize when screen mode initially set.
	void Renderer::Initialize()
	{
		defaultRenderPath_ = new RenderPath();
//		defaultRenderPath_->AddCommand(new RenderPass(CMD_CLEAR));
		defaultRenderPath_->AddCommand(new ScenePass());
		
		Subscribe(&Renderer::HandleStartup);
		Subscribe(&Renderer::HandleRenderUpdate);
		Subscribe(&Renderer::HandlePostRenderUpdate);
		Subscribe(&Renderer::HandleEndFrame);
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
			
	void Renderer::HandleStartup(const Startup& eventData)
	{
		LoadShaders();
		
		auto& cache = GetSubsystem<ResourceCache>();

		Model* m = new Model();
		m->SetName("Cube");
		m->SetNumGeometries(1);
		m->SetNumGeometryLodLevels(0, 1);

		// Layout of this structure matches the one we defined in pipeline state
        struct Vertex
        {
            float3 pos;
            uint color;
        };

        Vector<Vertex> CubeVerts =
        {
            {float3(-1,-1,-1), Color(1,0,0,1).ToUInt()},
            {float3(-1,+1,-1), Color(0,1,0,1).ToUInt()},
            {float3(+1,+1,-1), Color(0,0,1,1).ToUInt()},
            {float3(+1,-1,-1), Color(1,1,1,1).ToUInt()},

            {float3(-1,-1,+1), Color(1,1,0,1).ToUInt()},
            {float3(-1,+1,+1), Color(0,1,1,1).ToUInt()},
            {float3(+1,+1,+1), Color(1,0,1,1).ToUInt()},
            {float3(+1,-1,+1), Color(0.2f,0.2f,0.2f,1).ToUInt()},
        };
		  
		Vector<ushort> Indices =
        {
            2,0,1, 2,3,0,
            4,6,5, 4,7,6,
            0,7,4, 0,3,7,
            1,0,4, 1,4,5,
            1,5,2, 5,6,2,
            3,6,7, 3,2,6
        };

		SPtr<VertexBuffer> pVertexBuffer(new VertexBuffer());
		pVertexBuffer->Create(std::move(CubeVerts));
		SPtr<IndexBuffer> pIndexBuffer(new IndexBuffer(std::move(Indices)));

		SPtr<Geometry> geo(new Geometry());
		geo->SetVertexBuffer(0, pVertexBuffer);
		geo->SetIndexBuffer(pIndexBuffer);
		geo->SetDrawRange(PrimitiveTopology::TRIANGLE_LIST, 0, pIndexBuffer->GetIndexCount());
		m->SetGeometry(0, 0, geo);
		m->SetBoundingBox(BoundingBox(-1, 1));
		cache.RegisterResource(m);

		CreateInstancingBuffer();
	}
	
	void Renderer::LoadShaders()
	{
		auto& cache = GetSubsystem<ResourceCache>();

		defaultMaterial_ = new Material();
		defaultMaterial_->SetName("DefaultMaterial");
		defaultMaterial_->SetShaderAttr(ResourceRef::Create<Shader>("Shaders/Textured.shader"));
		
		SPtr<Texture> defaultTexture(new Texture());
		int w = 1;
		int h = 1;

		Vector<Vector<byte>> mip(1);	
		Vector<byte>& subData = mip.back();
		subData.resize(w * h * 4, 255);
		subData[1] = 0;

		TextureDesc desc;
		desc.Type = Diligent::RESOURCE_DIM_TEX_2D;
		desc.Width = (uint)w;
		desc.Height = (uint)h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = Diligent::TEX_FORMAT_RGBA8_UNORM;
		desc.SampleCount = 1;
		desc.Usage = Diligent::USAGE_STATIC;
		desc.BindFlags = Diligent::BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		
		defaultTexture->SetName("Magenta");
		defaultTexture->Create(desc, std::move(mip));

		cache.RegisterResource(defaultTexture);

		defaultMaterial_->SetTexture("DiffMap", defaultTexture);

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

		if (drawDebug_)
		{
			DrawDebugGeometry(true);
		}
	}

	void Renderer::HandlePostRenderUpdate(const PostRenderUpdate& eventData)
	{
		auto& views = MainContext(views_);

		for (auto view : views)
		{
			view->PostUpdate();
		}

	}

	void Renderer::HandleEndFrame(const EndFrame& args)
	{
		graphics_.Frame();
	}
	
	void Renderer::Render()
	{
		if (!graphics_.BeginRender())
		{
			return;
		}
		
		auto& views = RenderContext(views_);

		//if (views.empty())
		{		
			graphics_.Clear(nullptr, Color::GRAY, CLEAR_COLOR |CLEAR_DEPTH, 1.0f, 0xff);
		}
		
		for(int i = 0; i < views.size(); i++)
		{
			auto& view = views[i];
			if(view.Expired())
			{
				continue;
			}

			view->Render();
		}
		
		auto& batchQueue = RenderContext(batchQueue_);
		if(!batchQueue.empty())
		{
			for(int i = 0; i < batchQueue.size(); i++)
			{
				auto& batch = batchQueue[i];
				batch.Draw();
			}

			batchQueue.clear();
		}

		RenderContext(scissorRect_).resize(1);
		graphics_.EndRender();
	}

	void Renderer::DrawDebugGeometry(bool depthTest)
	{
		UNIQUE_PROFILE(RendererDrawDebug);

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
			for (size_t i = 0; i < geometries.size(); ++i)
			{
				if (!processedGeometries.count(geometries[i]))
				{
					geometries[i]->DrawDebugGeometry(debug, depthTest);
					processedGeometries.insert(geometries[i]);
				}
			}
				
			{
				const PODVector<Light*>& lights = view->dirLights_;
				for (size_t i = 0; i < lights.size(); ++i)
				{
					if (!processedLights.count(lights[i]))
					{
						lights[i]->DrawDebugGeometry(debug, depthTest);
						processedLights.insert(lights[i]);
					}
				}	
			}

			{
				const PODVector<Light*>& lights = view->pointLights_;
				for (size_t i = 0; i < lights.size(); ++i)
				{
					if (!processedLights.count(lights[i]))
					{
						lights[i]->DrawDebugGeometry(debug, depthTest);
						processedLights.insert(lights[i]);
					}
				}
			}
	
			{
				const PODVector<Light*>& lights = view->spotLights_;
				for (size_t i = 0; i < lights.size(); ++i)
				{
					if (!processedLights.count(lights[i]))
					{
						lights[i]->DrawDebugGeometry(debug, depthTest);
						processedLights.insert(lights[i]);
					}
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

	void Renderer::Stop()
	{
		graphics_.Close();
	}

	void Renderer::SetBatchShaders(Batch& batch, uint passIndex, bool allowShadows, const BatchQueue& queue)
	{
		const String& geoDef = batch.geometryType_ ? ShaderUtil::interDefs[batch.geometryType_ - 1] : "";
		batch.pipelineState_ = batch.material_->GetPipeline(passIndex, geoDef);
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

	ushort Renderer::CacheScissor(int left, int top, int right, int bottom)
	{
		auto& rects = MainContext(scissorRect_);
		rects.emplace_back(left, top, right, bottom);
		return (ushort)rects.size() - 1;
	}

	IntRect& Renderer::GetScissor(ushort handler)
	{
		auto& rects = RenderContext(scissorRect_);
		return rects[handler];
	}
	
	Batch& Renderer::AddBatch(Geometry* geometry, Material* material, const Matrix3x4* worldTransform)
	{
		auto& batchQueue = MainContext(batchQueue_);
		batchQueue.emplace_back(geometry, material, worldTransform);
		return batchQueue.back();
	}

	Batch& Renderer::AddBatch(const Batch& batch)
	{
		auto& batchQueue = MainContext(batchQueue_);
		batchQueue.emplace_back(batch);
		return batchQueue.back();
	}
}