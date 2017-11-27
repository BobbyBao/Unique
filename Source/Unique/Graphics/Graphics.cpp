#include "Precompiled.h"
#include "Graphics.h"
#include "Texture.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"

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
	LLGL::RenderContext*			graphicsContext = nullptr;
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
			LLGL::RenderSystem::Unload(std::move(renderer));
		}
	}

	Window* Graphics::Initialize(const std::string& rendererModule, const LLGL::Size& size)
	{
		// Create render system
		renderer = LLGL::RenderSystem::Load(
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

		graphicsContext = renderer->CreateRenderContext(contextDesc);

		// Create command buffer
		commands = renderer->CreateCommandBuffer();

		const LLGL::ColorRGBAf defaultClearColor{ 0.1f, 0.1f, 0.4f };

		// Initialize command buffer
		commands->SetClearColor(defaultClearColor);
		commands->SetRenderTarget(*graphicsContext);
		commands->SetViewport({ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) });
		commands->SetScissor({ 0, 0, size.x, size.y });

		// Print renderer information
		const auto& info = renderer->GetRendererInfo();

		std::cout << "renderer information:" << std::endl;
		std::cout << "  renderer:         " << info.rendererName << std::endl;
		std::cout << "  device:           " << info.deviceName << std::endl;
		std::cout << "  vendor:           " << info.vendorName << std::endl;
		std::cout << "  shading language: " << info.shadingLanguageName << std::endl;

		FrameNoRenderWait();

		return &(Window&)graphicsContext->GetSurface();
	}

	void Graphics::Resize(const LLGL::Size& size)
	{
		auto videoMode = graphicsContext->GetVideoMode();

		// Update video mode
		videoMode.resolution = size;
		graphicsContext->SetVideoMode(videoMode);

		SetRenderTarget(nullptr);

		SetViewport(Viewport(0, 0, (float)size.x, (float)size.y));

		// Update scissor
		SetScissor({ 0, 0, videoMode.resolution.x, videoMode.resolution.y });
	}

	const std::string Graphics::GetRenderName() const
	{
		return renderer->GetName();
	}

	void Graphics::SetDebug(bool val)
	{
		debugger_ = val;
	}

	const Size& Graphics::GetResolution() const
	{
		return graphicsContext->GetVideoMode().resolution;
	}

	bool Graphics::IsOpenGL() const
	{
		return (renderer->GetRendererID() == LLGL::RendererID::OpenGL);
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


	void Graphics::AddCommand(std::function<void()> cmd)
	{
		preComands_.push_back(cmd);
	}

	void Graphics::PostCommand(std::function<void()> cmd)
	{
		postComands_.push_back(cmd);
	}

	void Graphics::ExecuteCommands(CommandQueue& cmds)
	{
		if (!cmds.empty())
		{
			for (auto& fn : cmds)
			{
				fn();
			}

			cmds.clear();
		}
	}

	void Graphics::BeginFrame()
	{
	}

	void Graphics::EndFrame()
	{
		RenderSemWait();

		FrameNoRenderWait();
	}

	void Graphics::BeginRender()
	{

	}

	void Graphics::EndRender()
	{
		graphicsContext->Present();

		if (MainSemWait())
		{
			{
				UNIQUE_PROFILE(ExecCommandsPre);
				ExecuteCommands(preComands_);
			}

			{
				UNIQUE_PROFILE(ExecCommandsPost);
				ExecuteCommands(postComands_);
			}

			RenderSemPost();

		}
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
			commands->SetRenderTarget(*graphicsContext);
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

	void Graphics::SwapContext()
	{
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

	void Graphics::Close()
	{
		MainSemWait();
		RenderSemPost();
	}

}

