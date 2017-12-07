#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <memory>
#include <iomanip>
#include <Windows.h>

#include "RefCntAutoPtr.h"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"

#include <RenderDeviceFactoryD3D11.h>
//#include <RenderDeviceFactoryD3D12.h>
#include <RenderDeviceFactoryOpenGL.h>


using namespace Diligent;

// Create Direct3D device and swap chain
void InitDevice(HWND hWnd, IRenderDevice **ppRenderDevice, IDeviceContext **ppImmediateContext, ISwapChain **ppSwapChain, DeviceType DevType)
{
	SwapChainDesc SCDesc;
	SCDesc.SamplesCount = 1;
	switch (DevType)
	{
	case DeviceType::D3D11:
	{
		EngineD3D11Attribs DeviceAttribs;
		DeviceAttribs.DebugFlags = (Uint32)EngineD3D11DebugFlags::VerifyCommittedShaderResources |
			(Uint32)EngineD3D11DebugFlags::VerifyCommittedResourceRelevance;

#ifdef ENGINE_DLL
		GetEngineFactoryD3D11Type GetEngineFactoryD3D11 = nullptr;
		// Load the dll and import GetEngineFactoryD3D11() function
		LoadGraphicsEngineD3D11(GetEngineFactoryD3D11);
#endif
		auto *pFactoryD3D11 = GetEngineFactoryD3D11();
		pFactoryD3D11->CreateDeviceAndContextsD3D11(DeviceAttribs, ppRenderDevice, ppImmediateContext, 0);
		pFactoryD3D11->CreateSwapChainD3D11(*ppRenderDevice, *ppImmediateContext, SCDesc, hWnd, ppSwapChain);
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

#endif
	case DeviceType::OpenGL:
	{
#ifdef ENGINE_DLL
		// Declare function pointer
		GetEngineFactoryOpenGLType GetEngineFactoryOpenGL = nullptr;
		// Load the dll and import GetEngineFactoryOpenGL() function
		LoadGraphicsEngineOpenGL(GetEngineFactoryOpenGL);
#endif
		EngineCreationAttribs EngineCreationAttribs;
		GetEngineFactoryOpenGL()->CreateDeviceAndSwapChainGL(
			EngineCreationAttribs, ppRenderDevice, ppImmediateContext, SCDesc, hWnd, ppSwapChain);
	}
	break;

	default:
	//	LOG_ERROR_AND_THROW("Unknown device type");
		break;
	}
}

int done = 0;
void
loop()
{
	SDL_Event event;
	/* Check for events */
	/*SDL_WaitEvent(&event); emscripten does not like waiting*/

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			//PrintKey(&event.key.keysym, (event.key.state == SDL_PRESSED) ? SDL_TRUE : SDL_FALSE, (event.key.repeat) ? SDL_TRUE : SDL_FALSE);
			break;
		case SDL_TEXTEDITING:
			//PrintText("EDIT", event.text.text);
			break;
		case SDL_TEXTINPUT:
			//PrintText("INPUT", event.text.text);
			break;
		case SDL_MOUSEBUTTONDOWN:
			/* Any button press quits the app... */
		case SDL_QUIT:
			done = 1;
			break;
		default:
			break;
		}
	}
#ifdef __EMSCRIPTEN__
	if (done) {
		emscripten_cancel_main_loop();
	}
#endif
}

static HWND GetWindowHandle(SDL_Window* window)
{
	SDL_SysWMinfo sysInfo;

	SDL_VERSION(&sysInfo.version);
	SDL_GetWindowWMInfo(window, &sysInfo);
	return sysInfo.info.win.window;
}

ISwapChain *g_pSwapChain;

int
main(int argc, char *argv[])
{
	SDL_Window *window;

	/* Enable standard application logging */
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

	/* Initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return (1);
	}

	/* Set 640x480 video mode */
	window = SDL_CreateWindow("Unique Test",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480, 0);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create 640x480 window: %s\n",
			SDL_GetError());

		SDL_Quit();
		return 2;
	}

#if __IPHONEOS__
	/* Creating the context creates the view, which we need to show keyboard */
	SDL_GL_CreateContext(window);
#endif

	RefCntAutoPtr<IRenderDevice> pRenderDevice;
	RefCntAutoPtr<IDeviceContext> pDeviceContext;
	Diligent::RefCntAutoPtr<ISwapChain> pSwapChain;
	DeviceType DevType = DeviceType::OpenGL;
	DevType = DeviceType::D3D11;
	InitDevice(GetWindowHandle(window), &pRenderDevice, &pDeviceContext, &pSwapChain, DevType);
	g_pSwapChain = pSwapChain;
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, 1);
#else
	while (!done) {
		loop();
		pSwapChain->Present();
	}
#endif

	SDL_Quit();
    return 0;
}

