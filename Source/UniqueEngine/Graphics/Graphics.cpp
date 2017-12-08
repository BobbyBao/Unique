#include "Precompiled.h"
#include <SDL/SDL.h>
#include "Graphics.h"
#include "Texture.h"
#include "GraphicsBuffer.h"
#include <iostream>

namespace Unique
{
	uObject(Graphics)
	{
		uFactory("Graphics")
	}
	
	IRenderDevice* renderDevice = nullptr;
	IDeviceContext* deviceContext = nullptr;

	int Graphics::currentContext_ = 0;
	CommandQueue Graphics::comands_;

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
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,	size.x_, size.y_, SDL_WINDOW_RESIZABLE);
		
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

		FrameNoRenderWait();

		return true;
	}

	void Graphics::Resize(const IntVector2& size)
	{
		auto fn = [=]()
		{
			resolution_ = size;
			swapChain_->Resize(size.x_, size.y_);
			deviceContext_->Flush();
		};

		if (Thread::IsMainThread())
		{
			Graphics::AddCommand(fn);
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
		swapChain_->Present();

		if (MainSemWait())
		{
			{
				UNIQUE_PROFILE(ExecCommands);
				ExecuteCommands(comands_);
			}

			SwapContext();

			RenderSemPost();

		}
	}

	void Graphics::Close()
	{
		MainSemWait();
		//FrameNoRenderWait();
		RenderSemPost();
	}

	void Graphics::AddCommand(std::function<void()> cmd)
	{
		assert(Context::IsMainThread());
		comands_.push_back(cmd);
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

	
	void Graphics::SwapContext()
	{
		currentContext_ = 1 - currentContext_;
	}

	void Graphics::FrameNoRenderWait()
	{
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

