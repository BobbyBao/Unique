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
		resolution_ = size;
		deviceType_ = deviceType;

		SDL_Window *window = SDL_CreateWindow("Unique Test",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	size.x_, size.y_, 0);
		
		if (!window) 
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s\n",
				SDL_GetError());

			SDL_Quit();
			return false;
		}

		InitDevice(window, &renderDevice_, &deviceContext_, &swapChain_, deviceType);
		
		renderDevice = renderDevice_;
		deviceContext = deviceContext_;

		GraphicsContext::FrameNoRenderWait();

		return true;
	}

	void Graphics::Resize(const IntVector2& size)
	{
		auto fn = [=]()
		{
			resolution_ = size;
			swapChain_->Resize(size.x_, size.y_);
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
		swapChain_->Present();

		GraphicsContext::EndRender();
	}

	void Graphics::Close()
	{
		GraphicsContext::Stop();
	}

}

