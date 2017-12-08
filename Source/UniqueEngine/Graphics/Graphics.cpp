#include "Precompiled.h"
#include "Graphics.h"
#include "Texture.h"
#include "GraphicsBuffer.h"
#include "GraphicsContext.h"
#include <iostream>
#include <SDL/SDL.h>

namespace Unique
{
	uObject(Graphics)
	{
		uFactory("Graphics")
	}
	
	IRenderDevice* renderDevice = nullptr;
	IDeviceContext* deviceContext = nullptr;

	extern void InitDevice(SDL_Window* window, IRenderDevice **ppRenderDevice, IDeviceContext **ppImmediateContext, ISwapChain **ppSwapChain, DeviceType DevType);
	
	Graphics::Graphics()
	{
	}

	Graphics::~Graphics()
	{
	}

	bool Graphics::Initialize(const IntVector2& size, DeviceType deviceType)
	{
		SDL_Window *window = SDL_CreateWindow("Unique Test",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	size.x_, size.y_, 0);
		
		if (!window) 
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s\n",
				SDL_GetError());

			SDL_Quit();
			return false;
		}

		InitDevice(window, &pRenderDevice, &pDeviceContext, &pSwapChain, deviceType);

		GraphicsContext::FrameNoRenderWait();

		return true;
	}

	void Graphics::Resize(const IntVector2& size)
	{
		auto fn = [=]()
		{
			pSwapChain->Resize(size.x_, size.y_);
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
	}

	void Graphics::EndRender()
	{
		pSwapChain->Present();

		GraphicsContext::EndRender();
	}

	void Graphics::Clear(uint flags)
	{
	//	commands->Clear(flags);
	}
	/*
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

	void Graphics::SetVertexBuffers(BufferArray* buffers)
	{
		commands->SetVertexBufferArray(*buffers);
	}

	void Graphics::SetIndexBuffer(IndexBuffer* buffer)
	{
		commands->SetIndexBuffer(*buffer);
	}

	void Graphics::SetTexture(Texture* texture, uint slot, long shaderStageFlags)
	{
	//	commands->SetTexture(*texture, slot, shaderStageFlags);
	//	commands->SetSampler(texture->sampler(), slot);
	}*/

	void Graphics::Draw(unsigned int numVertices, unsigned int firstVertex)
	{
	//	commands->Draw(numVertices, firstVertex);
	}

	void Graphics::DrawIndexed(unsigned int numIndexes, unsigned int firstIndex)
	{
	//	commands->DrawIndexed(numIndexes, firstIndex);
	}

	void Graphics::DrawIndexed(unsigned int numIndexes, unsigned int firstIndex, int vertexOffset)
	{
	//	commands->DrawIndexed(numIndexes, firstIndex, vertexOffset);
	}

	void Graphics::DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances)
	{
	//	commands->DrawInstanced(numVertices, firstVertex, numInstances);
	}

	void Graphics::DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances, unsigned int instanceOffset)
	{
	//	commands->DrawInstanced(numVertices, firstVertex, numInstances, instanceOffset);
	}

	void Graphics::DrawIndexedInstanced(unsigned int numVerticesPerInstance, unsigned int numInstances, unsigned int firstIndex)
	{
	//	commands->DrawIndexedInstanced(numVerticesPerInstance, numInstances, firstIndex);
	}

	void Graphics::DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset)
	{
	//	commands->DrawIndexedInstanced(numVertices, numInstances, firstIndex, vertexOffset);
	}

	void Graphics::DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset, unsigned int instanceOffset)
	{
	//	commands->DrawIndexedInstanced(numVertices, numInstances, firstIndex, vertexOffset, instanceOffset);
	}

	void Graphics::Dispatch(unsigned int groupSizeX, unsigned int groupSizeY, unsigned int groupSizeZ)
	{
	//	commands->Dispatch(groupSizeX, groupSizeY, groupSizeZ);
	}

	void Graphics::Close()
	{
		GraphicsContext::Stop();
	}

}

