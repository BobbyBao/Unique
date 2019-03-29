
#ifdef PLATFORM_WIN32

#include <memory>
#include <iomanip>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include "RefCntAutoPtr.h"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"

#include <EngineFactoryD3D11.h>
//#include <RenderDeviceFactoryD3D12.h>
//#include <RenderDeviceFactoryOpenGL.h>
#include "../GraphicsDefs.h"
#include <Windows.h>

using namespace Diligent;

namespace Unique
{


	static HWND GetWindowHandle(SDL_Window* window)
	{
		SDL_SysWMinfo sysInfo;

		SDL_VERSION(&sysInfo.version);
		SDL_GetWindowWMInfo(window, &sysInfo);
		return sysInfo.info.win.window;
	}

	// Create Direct3D device and swap chain
	void CreateDevice(SDL_Window* window, Unique::DeviceType DevType, const SwapChainDesc& SCDesc, IRenderDevice **ppRenderDevice, IDeviceContext **ppImmediateContext, ISwapChain **ppSwapChain)
	{
		HWND hWnd = GetWindowHandle(window);

		switch (DevType)
		{
		case DeviceType::D3D11:
		{
			EngineD3D11CreateInfo DeviceAttribs;
			DeviceAttribs.DebugFlags = (unsigned)EngineD3D11DebugFlags::VerifyCommittedShaderResources |
				(unsigned)EngineD3D11DebugFlags::VerifyCommittedResourceRelevance;

			FullScreenModeDesc fullScreenModeDesc;
#ifdef ENGINE_DLL
			GetEngineFactoryD3D11Type GetEngineFactoryD3D11 = nullptr;
			// Load the dll and import GetEngineFactoryD3D11() function
			LoadGraphicsEngineD3D11(GetEngineFactoryD3D11);
#endif
			auto *pFactoryD3D11 = GetEngineFactoryD3D11();
			pFactoryD3D11->CreateDeviceAndContextsD3D11(DeviceAttribs, ppRenderDevice, ppImmediateContext);
			pFactoryD3D11->CreateSwapChainD3D11(*ppRenderDevice, *ppImmediateContext, SCDesc, fullScreenModeDesc, hWnd, ppSwapChain);
		}
		break;
#if false
		case DeviceType::D3D12:
		{
#ifdef ENGINE_DLL
			GetEngineFactoryD3D12Type GetEngineFactoryD3D12 = nullptr;
			// Load the dll and import GetEngineFactoryD3D12() function
			LoadGraphicsEngineD3D12(GetEngineFactoryD3D12);
#endif

			auto *pFactoryD3D12 = GetEngineFactoryD3D12();
			EngineD3D12Attribs EngD3D12Attribs;
			EngD3D12Attribs.GPUDescriptorHeapDynamicSize[0] = 32768;
			EngD3D12Attribs.GPUDescriptorHeapSize[1] = 128;
			EngD3D12Attribs.GPUDescriptorHeapDynamicSize[1] = 2048 - 128;
			EngD3D12Attribs.DynamicDescriptorAllocationChunkSize[0] = 32;
			EngD3D12Attribs.DynamicDescriptorAllocationChunkSize[1] = 8; // D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
			pFactoryD3D12->CreateDeviceAndContextsD3D12(EngD3D12Attribs, ppRenderDevice, ppImmediateContext, 0);
			pFactoryD3D12->CreateSwapChainD3D12(*ppRenderDevice, *ppImmediateContext, SCDesc, hWnd, ppSwapChain);
		}
		break; 

		case DeviceType::OpenGL:
		{
#ifdef ENGINE_DLL
			// Declare function pointer
			GetEngineFactoryOpenGLType GetEngineFactoryOpenGL = nullptr;
			// Load the dll and import GetEngineFactoryOpenGL() function
			LoadGraphicsEngineOpenGL(GetEngineFactoryOpenGL);
#endif
			EngineCreateInfo EngineCreationAttribs;
			GetEngineFactoryOpenGL()->CreateDeviceAndSwapChainGL(
				EngineCreationAttribs, ppRenderDevice, ppImmediateContext, SCDesc, hWnd, ppSwapChain);
		}
		break;
#endif

		default:
			//	LOG_ERROR_AND_THROW("Unknown device type");
			break;
		}
	}
	
}

#endif