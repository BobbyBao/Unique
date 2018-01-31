#include "UniquePCH.h"
#include "Graphics.h"
#include "Texture.h"
#include <iostream>
#include <Errors.h>
#include <SDL/SDL.h>
#include "PipelineState.h"
#include "VertexBuffer.h"
#include "Geometry.h"
#include <RefCntAutoPtr.h>
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <SwapChain.h>
#include <GraphicsTypes.h>
#include <Buffer.h>
#include <BasicShaderSourceStreamFactory.h>
#include <Shader.h>
#include <TextureView.h>
#include <Sampler.h>

using namespace Diligent;

namespace Unique
{
	uObject(Graphics)
	{
		uFactory("Graphics")
	}
	
	ThreadID Graphics::renderThreadID = 0;
	int Graphics::currentContext_ = 0;
	int Graphics::currentFrame_ = 0;
	bool Graphics::singleThreaded_ = false;
	Semaphore Graphics::renderSem_;
	Semaphore Graphics::mainSem_;
	long long Graphics::waitSubmit_ = 0;
	long long Graphics::waitRender_ = 0;
	CommandQueue Graphics::comands_;
	CommandQueue Graphics::postComands_;

	struct Impl
	{
		Diligent::RefCntAutoPtr<IRenderDevice> renderDevice_;
		Diligent::RefCntAutoPtr<IDeviceContext> deviceContext_;
		Diligent::RefCntAutoPtr<ISwapChain> swapChain_;
		Diligent::RefCntAutoPtr<Diligent::IResourceMapping> resourceMapping_;
	};

	extern void CreateDevice(SDL_Window* window, DeviceType DevType, const SwapChainDesc& SCDesc, IRenderDevice **ppRenderDevice, IDeviceContext **ppImmediateContext, ISwapChain **ppSwapChain);
	
	Graphics::Graphics() : title_("Unique Engine"), impl_(*new Impl())
	{
		if (!GetContext()->RequireSDL(SDL_INIT_VIDEO))
		{
			UNIQUE_LOGERRORF("Couldn't initialize SDL: %s\n", SDL_GetError());
			return;
		}

		SetRenderThread();
	}

	Graphics::~Graphics()
	{
		GetContext()->ReleaseSDL();
	}

	bool Graphics::CreateWindow(const IntVector2& size, DeviceType deviceType)
	{
		resolution_ = size;
		deviceType_ = deviceType;

		window_ = SDL_CreateWindow(title_.CString(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			size.x_, size.y_, SDL_WINDOW_RESIZABLE);
		
		if (!window_)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s\n", SDL_GetError());
			SDL_Quit();
			return false;
		}

		
		return true;
	}

	bool Graphics::Initialize()
	{
		uCall
		(
			SwapChainDesc SCDesc;
			SCDesc.SamplesCount = 1/*multiSampling_*/;
			SCDesc.ColorBufferFormat = srgb_ ? TEX_FORMAT_RGBA8_UNORM_SRGB : TEX_FORMAT_RGBA8_UNORM;
			SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;
			CreateDevice(window_, deviceType_, SCDesc, &impl_.renderDevice_, &impl_.deviceContext_, &impl_.swapChain_);
		
			impl_.renderDevice_->CreateResourceMapping(ResourceMappingDesc(), &impl_.resourceMapping_);
			inited_ = true;
		);

		FrameNoRenderWait();

		Frame();

		if(!inited_)
		{
			UNIQUE_LOGERROR("Create render failed.");
		}
		
		GetSubsystem<ResourceCache>().AddResourceDir("CoreData\\Shaders");
		GetSubsystem<ResourceCache>().AddResourceDir("CoreData\\Shaders\\Include");
		GetSubsystem<ResourceCache>().AddResourceDir("CoreData\\Shaders\\HLSL");
		return true;
	}

	void Graphics::Resize(const IntVector2& size)
	{
		resolution_ = size;

		ScreenMode eventData;
		eventData.width_ = size.x_;
		eventData.height_ = size.y_;
// 			eventData.fullscreen_ = fullscreen_;
// 			eventData.resizable_ = resizable_;
// 			eventData.borderless_ = borderless_;
// 			eventData.highDPI_ = highDPI_;
		SendEvent(eventData);

		uCall
		(
			impl_.swapChain_->Resize(size.x_, size.y_);
			impl_.deviceContext_->Flush();
		);
		
	}

	void Graphics::SetTitle(const String& title)
	{
		title_ = title;

		if (window_)
		{
			SDL_SetWindowTitle(window_, title_.CString());
		}
	}

	DeviceType Graphics::GetDeviceType() const
	{
		return deviceType_;
	}

	void Graphics::SetDebug(bool val)
	{
		debugger_ = val;
	}

	const IntVector2& Graphics::GetResolution() const
	{
		return resolution_;
	}

	bool Graphics::IsDirect3D() const
	{
		return (deviceType_ == DeviceType::D3D11 || deviceType_ == DeviceType::D3D12);
	}

	bool Graphics::IsOpenGL() const
	{
		return (deviceType_ == DeviceType::OpenGL || deviceType_ == DeviceType::OpenGLES);
	}

	void Graphics::AddResource(const char *Name, GPUObject* pObject, bool bIsUnique)
	{
		uCall
		(
			impl_.resourceMapping_->AddResource(Name, *pObject, bIsUnique);
		);
	}

	void Graphics::RemoveResourceByName(const char *Name, uint ArrayIndex)
	{
		uPost
		(
			impl_.resourceMapping_->RemoveResourceByName(Name, ArrayIndex);
		);
	}

	void Graphics::Frame()
	{
		RenderSemWait();

		FrameNoRenderWait();
	}

	bool Graphics::IsRenderThread()
	{
		return renderThreadID == Thread::GetCurrentThreadID();
	}
	
	void Graphics::SetRenderThread()
	{
		renderThreadID = Thread::GetCurrentThreadID();
	}

	void Graphics::CreateBuffer(GraphicsBuffer& buffer, const ByteArray& data)
	{
		BufferDesc buffDesc;
		buffDesc.BindFlags = buffer.bindFlags_;
		buffDesc.Usage = (Diligent::USAGE)buffer.usage_;
		buffDesc.ElementByteStride = buffer.stride_;
		buffDesc.uiSizeInBytes = buffer.sizeInBytes_;
		buffDesc.CPUAccessFlags = buffer.cpuAccessFlags_;

		BufferData buffData;
		buffData.pData = data.data();
		buffData.DataSize = (uint)data.size();

		impl_.renderDevice_->CreateBuffer(buffDesc, buffData, buffer);
	}

	void Graphics::CreateShader(ShaderVariation& shader)
	{
		ShaderCreationAttribs Attrs;
		Attrs.Desc.Name = shader.shaderStage_.source_.CString();// owner_.GetName().CString();
		Attrs.Macros = (const Diligent::ShaderMacro*)(const ShaderMacro*)shader.macros_;
		Attrs.FilePath = shader.shaderStage_.source_;
		Attrs.EntryPoint = shader.shaderStage_.entryPoint_;
		Attrs.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
		//Attrs.Source = source_.CString();
		Attrs.Desc.ShaderType = (Diligent::SHADER_TYPE)shader.shaderStage_.shaderType_;
		Attrs.Desc.TargetProfile = (Diligent::SHADER_PROFILE)SHADER_PROFILE_DX_4_0;
		Attrs.Desc.VariableDesc = (const Diligent::ShaderVariableDesc *)shader.shaderVariableDesc_.data();
		Attrs.Desc.NumVariables = (uint)shader.shaderVariableDesc_.size();

		class ShaderSourceStream : public Diligent::ObjectBase<IFileStream>
		{
		public:
			ShaderSourceStream(IReferenceCounters *pRefCounters, const char* Name)
				: Diligent::ObjectBase<IFileStream>(pRefCounters)
			{
				file_ = GetSubsystem<ResourceCache>().GetFile(Name);
			}
			
			virtual void QueryInterface( const INTERFACE_ID &IID, IObject **ppInterface )override
			{
				*ppInterface = this;
				this->AddRef();
			}

			/// Reads data from the stream
			virtual bool Read( void *Data, size_t BufferSize )
			{
				return file_->Read(Data, BufferSize) == BufferSize;
			}

			virtual void Read( IDataBlob *pData )
			{    
				auto FileSize = GetSize();
				pData->Resize(FileSize);
				Read(pData->GetDataPtr(), pData->GetSize());
			}

			/// Writes data to the stream
			virtual bool Write( const void *Data, size_t Size )
			{
				return file_->Write(Data, Size) == Size;
			}

			virtual size_t GetSize()
			{
				return file_->GetSize();
			}

			virtual bool IsValid()
			{
				return file_->IsOpen();
			}
		private:
			SPtr<File> file_;
		};

		class ShaderSourceStreamFactory : public IShaderSourceInputStreamFactory
		{
		public:
			ShaderSourceStreamFactory()
			{
			}

			~ShaderSourceStreamFactory()
			{
			}

			virtual void CreateInputStream( const Char *Name, IFileStream **ppStream )override
			{
				Diligent::RefCntAutoPtr<ShaderSourceStream> pBasicFileStream;
				pBasicFileStream = MakeNewRCObj<ShaderSourceStream>()(Name);
				if( pBasicFileStream->IsValid() )
				{
					pBasicFileStream->QueryInterface( IID_FileStream, reinterpret_cast<IObject**>(ppStream) );
				}
				else
				{
					pBasicFileStream.Release();
					*ppStream = nullptr;
				}
			}

		private:
		};

		ShaderSourceStreamFactory BasicSSSFactory;
		Attrs.pShaderSourceStreamFactory = &BasicSSSFactory;

		try
		{
			impl_.renderDevice_->CreateShader(Attrs, shader);
		}
		catch(...)
		{
		}

	}

	void Graphics::CreateSampler(Sampler& sampler)
	{
		impl_.renderDevice_->CreateSampler(sampler.samplerDesc_, sampler);
	}
	
	void Graphics::CreateTexture(Texture& texture)
	{
		impl_.renderDevice_->CreateTexture(texture.desc_, texture.texData_, texture);
		
		CreateSampler(texture.sampler_);

		ITexture* textureObject = texture;

		if (texture.desc_.BindFlags & BIND_SHADER_RESOURCE)
		{
			ITextureView* tv = textureObject->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
			tv->SetSampler(texture.sampler_);
			texture.shaderResourceView_ = new TextureView(texture, tv);
			
		}

		if (texture.desc_.BindFlags & BIND_RENDER_TARGET)
		{
			texture.renderTargetView_ = new TextureView(texture, textureObject->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET));
		}

		if (texture.desc_.BindFlags & BIND_DEPTH_STENCIL)
		{
			texture.depthStencilView_ = new TextureView(texture, textureObject->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL));
		}

		if (texture.desc_.BindFlags & BIND_UNORDERED_ACCESS)
		{
			texture.unorderedAccessView_ = new TextureView(texture, textureObject->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS));
			
		}
	}
	
	void Graphics::CreatePipelineState(PipelineState& pipelineState)
	{
		impl_.renderDevice_->CreatePipelineState(pipelineState.psoDesc_, pipelineState);
		IPipelineState* pIPipelineState = pipelineState;
		pIPipelineState->BindShaderResources(impl_.resourceMapping_, BIND_SHADER_RESOURCES_ALL_RESOLVED);
		pIPipelineState->CreateShaderResourceBinding(&pipelineState.shaderResourceBinding_);
	}

	void Graphics::ReleaseDeviceObject(void* deviceObject)
	{
		((IDeviceObject*)deviceObject)->Release();
	}
	
	void* Graphics::Map(GraphicsBuffer* buffer, MapFlags mapFlags)
	{
		IBuffer* pBuffer = *buffer;
		void* bufferData = nullptr;
		pBuffer->Map(impl_.deviceContext_, MAP_WRITE, (uint)mapFlags, bufferData);
		return bufferData;
	}

	void Graphics::Unmap(GraphicsBuffer* buffer, MapFlags mapFlags)
	{
		IBuffer* pBuffer = *buffer;
		pBuffer->Unmap(impl_.deviceContext_, MAP_WRITE, (uint)mapFlags);
	}
	
	bool Graphics::BeginRender()
	{
		if(inited_)
		{
			impl_.swapChain_->Present();
		}

		if (MainSemWait())
		{
			UNIQUE_PROFILE(ExecCommands);
			ExecuteCommands(comands_);

			GPUObject::UpdateBuffers();
			return true;
		}

		return false;
	}

	void Graphics::EndRender()
	{
		{
			UNIQUE_PROFILE(ExecCommands);
			ExecuteCommands(postComands_);
		}
		
		RenderSemPost();
	}

	void Graphics::Clear(TextureView *pView, const Color& color, uint ClearFlags, float fDepth, byte Stencil)
	{	
		uint flags = ClearFlags >> 1;
		if(pView)
		{
			if(ClearFlags & CLEAR_COLOR_FLAG)
				impl_.deviceContext_->ClearRenderTarget(*pView, color);

			if(flags)
				impl_.deviceContext_->ClearDepthStencil(*pView, flags, fDepth, Stencil);
		}
		else
		{
			if(ClearFlags & CLEAR_COLOR_FLAG)
				impl_.deviceContext_->ClearRenderTarget(nullptr, color);

			if(flags)
				impl_.deviceContext_->ClearDepthStencil(nullptr, flags, fDepth, Stencil);
		}
	}
	
	void Graphics::SetScissorRects(uint NumRects, const IntRect *pRects, uint RTWidth, uint RTHeight)
	{
		impl_.deviceContext_->SetScissorRects(NumRects, (Diligent::Rect*)pRects, RTWidth, RTHeight);
	}
		
	void Graphics::Draw(Geometry* geometry, PipelineState* pipeline)
	{
		IBuffer *buffer[8] = { nullptr };
		uint offsets[8] = { 0 };
		uint strides[8] = { 0 };
		for (size_t i = 0; i < geometry->vertexBuffers_.size(); i++)
		{
			buffer[i] = *geometry->vertexBuffers_[i];
			offsets[i] = 0;
			strides[i] = geometry->vertexBuffers_[i]->GetStride();
		}
		
		impl_.deviceContext_->SetVertexBuffers(0, (uint)geometry->vertexBuffers_.size(), buffer, strides, offsets, SET_VERTEX_BUFFERS_FLAG_RESET);

		DrawAttribs drawAttribs;
		drawAttribs.Topology = (Diligent::PRIMITIVE_TOPOLOGY)geometry->primitiveType_;
		drawAttribs.IsIndexed = (geometry->indexBuffer_ != nullptr);
		
		if (geometry->indexBuffer_ && geometry->indexCount_ > 0)
		{
			drawAttribs.FirstIndexLocation = geometry->indexStart_;
			drawAttribs.NumIndices = geometry->indexCount_; 
			drawAttribs.IndexType = geometry->indexBuffer_->GetStride() == 4 ? (VALUE_TYPE)ValueType::VT_UINT32 : (VALUE_TYPE)ValueType::VT_UINT16;
			impl_.deviceContext_->SetIndexBuffer(*geometry->indexBuffer_, 0);
		}
		else if (geometry->vertexCount_ > 0)
		{
			drawAttribs.StartVertexLocation = geometry->vertexStart_;
			drawAttribs.NumVertices = geometry->vertexCount_;
		}

		impl_.deviceContext_->SetPipelineState((Diligent::IPipelineState*)pipeline->GetPipeline());

		auto& graphics = GetSubsystem<Graphics>();
		
		pipeline->GetShaderResourceBinding()->BindResources(SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL,
			impl_.resourceMapping_, BIND_SHADER_RESOURCES_UPDATE_UNRESOLVED | BIND_SHADER_RESOURCES_ALL_RESOLVED);
		impl_.deviceContext_->CommitShaderResources(pipeline->GetShaderResourceBinding(), COMMIT_SHADER_RESOURCES_FLAG_TRANSITION_RESOURCES);
		impl_.deviceContext_->Draw(drawAttribs);
	}
		
	void Graphics::Draw(Geometry* geometry, PipelineState* pipeline, PrimitiveTopology primitiveType, 
		unsigned vertexStart, unsigned vertexCount, unsigned indexStart, unsigned indexCount)
	{
		IBuffer *buffer[8] = { nullptr };
		uint offsets[8] = { 0 };
		uint strides[8] = { 0 };
		for (size_t i = 0; i < geometry->vertexBuffers_.size(); i++)
		{
			buffer[i] = *geometry->vertexBuffers_[i];
			offsets[i] = vertexStart * geometry->vertexBuffers_[i]->GetStride();
			strides[i] = geometry->vertexBuffers_[i]->GetStride();
		}

		impl_.deviceContext_->SetVertexBuffers(0, (uint)geometry->vertexBuffers_.size(), buffer, strides, offsets, SET_VERTEX_BUFFERS_FLAG_RESET);

		DrawAttribs drawAttribs;
		drawAttribs.Topology = (Diligent::PRIMITIVE_TOPOLOGY)primitiveType;
		drawAttribs.IsIndexed = (geometry->indexBuffer_ != nullptr);
		
		if (geometry->indexBuffer_ && geometry->indexCount_ > 0)
		{
			drawAttribs.FirstIndexLocation = indexStart;
			drawAttribs.NumIndices = indexCount;
			drawAttribs.IndexType = geometry->indexBuffer_->GetStride() == 4 ? (VALUE_TYPE)ValueType::VT_UINT32 : (VALUE_TYPE)ValueType::VT_UINT16;
			impl_.deviceContext_->SetIndexBuffer(*geometry->indexBuffer_, 0);
		}
		else if (geometry->vertexCount_ > 0)
		{
			drawAttribs.NumVertices = vertexCount;
		}

		impl_.deviceContext_->SetPipelineState((Diligent::IPipelineState*)pipeline->GetPipeline());

		auto& graphics = GetSubsystem<Graphics>();

		pipeline->GetShaderResourceBinding()->BindResources(SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL,
			impl_.resourceMapping_, BIND_SHADER_RESOURCES_UPDATE_UNRESOLVED | BIND_SHADER_RESOURCES_ALL_RESOLVED);

		impl_.deviceContext_->CommitShaderResources(pipeline->GetShaderResourceBinding(), COMMIT_SHADER_RESOURCES_FLAG_TRANSITION_RESOURCES);

		impl_.deviceContext_->Draw(drawAttribs);
	}

	void Graphics::DrawInstanced(Geometry* geometry, PipelineState* pipeline, uint instanceOffset, uint numInstances)
	{
		IBuffer *buffer[8] = { nullptr };
		uint offsets[8] = { 0 };
		uint strides[8] = { 0 };
		for (size_t i = 0; i < geometry->vertexBuffers_.size(); i++)
		{
			buffer[i] = *geometry->vertexBuffers_[i];
			offsets[i] = 0;
			strides[i] = geometry->vertexBuffers_[i]->GetStride();
		}

		DrawAttribs drawAttribs;
		drawAttribs.Topology = (Diligent::PRIMITIVE_TOPOLOGY)geometry->primitiveType_;
		drawAttribs.IsIndexed = (geometry->indexBuffer_ != nullptr);
		
		impl_.deviceContext_->SetVertexBuffers(0, (uint)geometry->vertexBuffers_.size(), buffer, strides, offsets, SET_VERTEX_BUFFERS_FLAG_RESET);

		if (geometry->indexBuffer_ && geometry->indexCount_ > 0)
		{
			drawAttribs.FirstIndexLocation = geometry->indexStart_;
			drawAttribs.NumIndices = geometry->indexCount_;
			drawAttribs.IndexType = geometry->indexBuffer_->GetStride() == 4 ? (VALUE_TYPE)ValueType::VT_UINT32 : (VALUE_TYPE)ValueType::VT_UINT16;
			impl_.deviceContext_->SetIndexBuffer(*geometry->indexBuffer_, 0);
		}
		else if (geometry->vertexCount_ > 0)
		{
			drawAttribs.NumVertices = geometry->vertexCount_;
		}

		drawAttribs.NumInstances = numInstances;
		drawAttribs.FirstInstanceLocation = instanceOffset;
		impl_.deviceContext_->SetPipelineState((Diligent::IPipelineState*)pipeline->GetPipeline());

		auto& graphics = GetSubsystem<Graphics>();

		pipeline->GetShaderResourceBinding()->BindResources(
			SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL, impl_.resourceMapping_,
			BIND_SHADER_RESOURCES_UPDATE_UNRESOLVED | BIND_SHADER_RESOURCES_ALL_RESOLVED);

		impl_.deviceContext_->CommitShaderResources(pipeline->GetShaderResourceBinding(), COMMIT_SHADER_RESOURCES_FLAG_TRANSITION_RESOURCES);

		impl_.deviceContext_->Draw(drawAttribs);
	}

	void Graphics::Close()
	{
		MainSemWait();
		RenderSemPost();
	}

	void Graphics::AddCommand(const std::function<void()>& cmd)
	{
		assert(Thread::IsMainThread());
		comands_.push_back(cmd);
	}
	
	void Graphics::PostCommand(const std::function<void()>& cmd)
	{
		assert(Thread::IsMainThread());
		postComands_.push_back(cmd);
	}

	void Graphics::ExecuteCommands(CommandQueue& cmds)
	{
		if (!cmds.empty())
		{
			//for (auto& fn : cmds)
			for(size_t i = 0; i < cmds.size(); i++)
			{
				auto&fn = cmds[i];
				fn();
			}

			cmds.clear();
		}
	}

	
	void Graphics::SwapContext()
	{
		currentFrame_++;
		currentContext_ = 1 - currentContext_;
		LOG_RENDER("===============SwapContext : ", currentContext_);
	}

	void Graphics::FrameNoRenderWait()
	{
		SwapContext();
		// release render thread
		MainSemPost();
	}

	void Graphics::MainSemPost()
	{
		if (!singleThreaded_)
		{
			mainSem_.post();
		}
	}

	bool Graphics::MainSemWait(int _msecs)
	{
		if (singleThreaded_)
		{
			return true;
		}

		UNIQUE_PROFILE(MainThreadWait);
		HiresTimer timer;
		bool ok = mainSem_.wait(_msecs);
		if (ok)
		{
			waitSubmit_ = timer.GetUSec(false);
			return true;
		}

		return false;
	}

	void Graphics::RenderSemPost()
	{
		if (!singleThreaded_)
		{
			renderSem_.post();
		}
	}

	void Graphics::RenderSemWait()
	{
		if (!singleThreaded_)
		{
			UNIQUE_PROFILE(RenderThreadWait);
			HiresTimer timer;
			bool ok = renderSem_.wait();
			assert(ok);
			waitRender_ = timer.GetUSec(false);
		}
	}
}

