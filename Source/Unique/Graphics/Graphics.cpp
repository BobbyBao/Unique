#include "Precompiled.h"
#include "Graphics.h"
#include "Texture.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "GraphicsContext.h"

namespace Unique
{
	uObject(Graphics)
	{
		uFactory("Graphics")
	}

	class Debugger : public LLGL::RenderingDebugger
	{
		void OnError(LLGL::ErrorType type, Message& message) override
		{
			std::cerr << "ERROR: " << message.GetSource() << ": " << message.GetText() << std::endl;
			message.Block();
		}

		void OnWarning(LLGL::WarningType type, Message& message) override
		{
			std::cerr << "WARNING: " << message.GetSource() << ": " << message.GetText() << std::endl;
			message.Block();
		}

	};

	// Render system
	UPtr<LLGL::RenderSystem>        renderer;
	// Main render context
	LLGL::RenderContext*			renderContext = nullptr;
	// Main command buffer
	LLGL::CommandBuffer*            commands = nullptr;

	Graphics::Graphics():
		profilerObj_ {	new LLGL::RenderingProfiler() },
		debuggerObj_{ new Debugger() }
	{
	}

	Graphics::~Graphics()
	{
		if (renderer)
		{
			renderer->Release(*renderContext);
			renderer->Release(*commands);

			LLGL::RenderSystem::Unload(std::move(renderer));
		}
	}

	Window* Graphics::Initialize(const std::string& rendererModule, const LLGL::Size& size)
	{
		// Create render system
		renderer = LLGL::RenderSystem::Load
		(
			rendererModule,
			(debugger_ ? profilerObj_.get() : nullptr),
			(debugger_ ? debuggerObj_.get() : nullptr)
		);

		// Create render context
		LLGL::RenderContextDescriptor contextDesc;
		{
			contextDesc.videoMode.resolution = size;
			contextDesc.vsync.enabled = vsync_;
			contextDesc.multiSampling.enabled = (multiSampling_ > 1);
			contextDesc.multiSampling.samples = multiSampling_;

#if 0
			contextDesc.profileOpenGL.extProfile = true;
			contextDesc.profileOpenGL.coreProfile = true;
			contextDesc.profileOpenGL.version = LLGL::OpenGLVersion::OpenGL_3_3;
			/*contextDesc.debugCallback = [](const std::string& type, const std::string& message)
			{
			std::cout << type << " -- " << message << std::endl;
			};*/
#endif

#ifdef __linux__
			contextDesc.profileOpenGL.extProfile = true;
			contextDesc.profileOpenGL.coreProfile = true;
			contextDesc.profileOpenGL.version = LLGL::OpenGLVersion::OpenGL_3_3;
#endif
		}

		renderContext = renderer->CreateRenderContext(contextDesc);

		// Create command buffer
		commands = renderer->CreateCommandBuffer();

		const LLGL::ColorRGBAf defaultClearColor{ 0.1f, 0.1f, 0.4f };

		// Initialize command buffer
		commands->SetClearColor(defaultClearColor);
		commands->SetRenderTarget(*renderContext);
		commands->SetViewport({ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) });
		commands->SetScissor({ 0, 0, size.x, size.y });

		// Print renderer information
		const auto& info = renderer->GetRendererInfo();

		std::cout << "renderer information:" << std::endl;
		std::cout << "  renderer:         " << info.rendererName << std::endl;
		std::cout << "  device:           " << info.deviceName << std::endl;
		std::cout << "  vendor:           " << info.vendorName << std::endl;
		std::cout << "  shading language: " << info.shadingLanguageName << std::endl;

		GraphicsContext::FrameNoRenderWait();

		return &(Window&)renderContext->GetSurface();
	}

	void Graphics::Resize(const LLGL::Size& size)
	{
		auto fn = [=]()
		{
			auto videoMode = renderContext->GetVideoMode();

			// Update video mode
			videoMode.resolution = size;
			renderContext->SetVideoMode(videoMode);

			SetRenderTarget(nullptr);

			SetViewport(Viewport(0, 0, (float)size.x, (float)size.y));

			// Update scissor
			SetScissor({ 0, 0, videoMode.resolution.x, videoMode.resolution.y });
		};

		if (Thread::IsMainThread())
		{
			GraphicsContext::AddCommand(fn);
		}
		else
		{
			fn();
		}

	}

	uint Graphics::GetRenderID() const
	{
		return renderer->GetRendererID();
	}

	void Graphics::SetDebug(bool val)
	{
		debugger_ = val;
	}

	const Size& Graphics::GetResolution() const
	{
		return renderContext->GetVideoMode().resolution;
	}

	bool Graphics::IsDirect3D() const
	{
		return (renderer->GetRendererID() == LLGL::RendererID::Direct3D9
			|| renderer->GetRendererID() == LLGL::RendererID::Direct3D10
			|| renderer->GetRendererID() == LLGL::RendererID::Direct3D11
		|| renderer->GetRendererID() == LLGL::RendererID::Direct3D12);
	}

	bool Graphics::IsOpenGL() const
	{
		return (renderer->GetRendererID() == LLGL::RendererID::OpenGL
			|| renderer->GetRendererID() == LLGL::RendererID::OpenGLES1
			|| renderer->GetRendererID() == LLGL::RendererID::OpenGLES2
			|| renderer->GetRendererID() == LLGL::RendererID::OpenGLES3);
	}

	SPtr<VertexBuffer> Graphics::CreateVertexBuffer(uint size, const LLGL::VertexFormat& vertexFormat, void* data)
	{
		SPtr<VertexBuffer> vb(new VertexBuffer());
		vb->handle_ = renderer->CreateBuffer(VertexBufferDesc(size * vertexFormat.stride, vertexFormat), data);
		return vb;
	}

	SPtr<IndexBuffer> Graphics::CreateIndexBuffer(uint size, const LLGL::IndexFormat& indexFormat, void* data)
	{
		SPtr<IndexBuffer> ib(new IndexBuffer());
		ib->handle_ = renderer->CreateBuffer(IndexBufferDesc(size * indexFormat.GetFormatSize(), indexFormat), data);
		return ib;
	}

	void Graphics::BeginFrame()
	{
		GraphicsContext::BeginFrame();
	}

	void Graphics::EndFrame()
	{
		GraphicsContext::EndFrame();
	}

	void Graphics::BeginRender()
	{
		GraphicsContext::BeginRender();
		profilerObj_->ResetCounters();
	}

	void Graphics::EndRender()
	{
		renderContext->Present();
		
		GraphicsContext::EndRender();
	}

	void Graphics::Clear(uint flags)
	{
		commands->Clear(flags);
	}

	void Graphics::SetRenderTarget(RenderTarget* renderTarget)
	{
		if (renderTarget)
		{
			commands->SetRenderTarget(*renderTarget);
		}
		else
		{
			commands->SetRenderTarget(*renderContext);
		}
	}

	void Graphics::SetViewport(const Viewport& viewport)
	{
		commands->SetViewport(viewport);
	}

	void Graphics::SetScissor(const Scissor& scissor)
	{
		commands->SetScissor(scissor);
	}

	void Graphics::SetGraphicsPipeline(GraphicsPipeline* graphicsPipeline)
	{
		commands->SetGraphicsPipeline(*graphicsPipeline);
	}

	void Graphics::SetComputePipeline(ComputePipeline* computePipeline)
	{
		commands->SetComputePipeline(*computePipeline);
	}

	void Graphics::SetVertexBuffer(VertexBuffer* buffer)
	{
		commands->SetVertexBuffer(*buffer);
	}

	void Graphics::SetIndexBuffer(IndexBuffer* buffer)
	{
		commands->SetIndexBuffer(*buffer);
	}

	void Graphics::SetTexture(Texture* texture, uint slot, long shaderStageFlags)
	{
		commands->SetTexture(*texture, slot, shaderStageFlags);
		commands->SetSampler(texture->sampler(), slot);
	}

	void Graphics::Draw(unsigned int numVertices, unsigned int firstVertex)
	{
		commands->Draw(numVertices, firstVertex);
	}

	void Graphics::DrawIndexed(unsigned int numVertices, unsigned int firstIndex)
	{
		commands->DrawIndexed(numVertices, firstIndex);
	}

	void Graphics::DrawIndexed(unsigned int numVertices, unsigned int firstIndex, int vertexOffset)
	{
		commands->DrawIndexed(numVertices, firstIndex, vertexOffset);
	}

	void Graphics::DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances)
	{
		commands->DrawInstanced(numVertices, firstVertex, numInstances);
	}

	void Graphics::DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances, unsigned int instanceOffset)
	{
		commands->DrawInstanced(numVertices, firstVertex, numInstances, instanceOffset);
	}

	void Graphics::DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex)
	{
		commands->DrawInstanced(numVertices, numInstances, firstIndex);
	}

	void Graphics::DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset)
	{
		commands->DrawIndexedInstanced(numVertices, numInstances, firstIndex, vertexOffset);
	}

	void Graphics::DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset, unsigned int instanceOffset)
	{
		commands->DrawIndexedInstanced(numVertices, numInstances, firstIndex, vertexOffset, instanceOffset);
	}

	void Graphics::Dispatch(unsigned int groupSizeX, unsigned int groupSizeY, unsigned int groupSizeZ)
	{
		commands->Dispatch(groupSizeX, groupSizeY, groupSizeZ);
	}


	void Graphics::Close()
	{
		GraphicsContext::Stop();
	}

}

