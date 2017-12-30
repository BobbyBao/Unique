#pragma once
#include "GraphicsDefs.h"
#include "Core/Object.h"
#include "Core/Semaphore.h"
#include <RefCntAutoPtr.h>
#include <RenderDevice.h>
#include <SwapChain.h>


namespace Unique
{
	using IRenderDevice = Diligent::IRenderDevice;
	using IDeviceContext = Diligent::IDeviceContext;
	using ISwapChain = Diligent::ISwapChain;

	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class GPUObject;
	
	using CommandQueue = Vector<std::function<void()> > ;

	struct TransientVertexBuffer
	{
		VertexBuffer* vertexBuffer_ = nullptr;
		uint offset_;
		uint count_;
	};

	struct TransientIndexBuffer
	{
		IndexBuffer* indexBuffer_ = nullptr;
		uint offset_;
		uint count_;
	};

	class Graphics : public Object
	{
		uRTTI(Graphics, Object)
	public:
		Graphics();
		~Graphics();
		
		bool Initialize(const IntVector2& size, DeviceType deviceType = DeviceType::D3D11);
		void Resize(const IntVector2& size);
		void SetTitle(const String& title);
		DeviceType GetDeviceType() const;
		void SetDebug(bool val);
		uint GetWidth() const { return resolution_.x_; }
		uint GetHeight() const { return resolution_.y_; }
		const IntVector2& GetResolution() const;

		// Returns the aspect ratio of the render context resolution (X:Y).
		inline float GetAspectRatio() const
		{
			return GetResolution().x_ / (float)GetResolution().y_;
		}

		bool IsDirect3D() const;
		bool IsOpenGL() const;

		//***MainThread***
		void Frame();

		void AddResource(const char *Name, GPUObject* pObject, bool bIsUnique = true);
		void AddResource(const char *Name, IDeviceObject *pObject, bool bIsUnique);   
		void AddResourceArray(const char *Name, uint StartIndex, IDeviceObject* const* ppObjects, uint NumElements, bool bIsUnique);
		void RemoveResourceByName(const char *Name, uint ArrayIndex = 0);
		void BindShaderResources(IPipelineState* pipelineState, uint flags);
		void BindResources(IShaderResourceBinding* shaderResourceBinding, uint shaderFlags, uint flags);
		//****************

		//Execute in render thread
		void BeginRender();
		void EndRender();
		void Close();

		static int currentContext_;
		inline static int GetRenderContext() { return 1 - currentContext_; }
		static void AddCommand(std::function<void()> cmd);
	protected:
		void ExecuteCommands(CommandQueue& cmds);
		void FrameNoRenderWait();
		void MainSemPost();
		bool MainSemWait(int _msecs = -1);
		void SwapContext();
		void RenderSemPost();
		void RenderSemWait();

		String title_;
		IntVector2 resolution_;
		DeviceType deviceType_;
		bool debugger_ = false;
		bool vsync_ = false;
		bool exit_ = false;
		int multiSampling_ = 4;
		Diligent::RefCntAutoPtr<IRenderDevice> renderDevice_;
		Diligent::RefCntAutoPtr<IDeviceContext> deviceContext_;
		Diligent::RefCntAutoPtr<ISwapChain> swapChain_;
		Diligent::RefCntAutoPtr<IResourceMapping> resourceMapping_;

		struct SDL_Window *window_ = nullptr;

		bool singleThreaded_ = false;
		Semaphore renderSem_;
		Semaphore mainSem_;
		long long waitSubmit_ = 0;
		long long waitRender_ = 0;

		static CommandQueue comands_;;
	};

	template<class T>
	inline T& MainContext(T* data)
	{
		return data[Graphics::currentContext_];
	}

	template<class T>
	inline T& RenderContext(T* data)
	{
		return data[1 - Graphics::currentContext_];
	}
	
	extern IRenderDevice* renderDevice;
	extern IDeviceContext* deviceContext;

#define uCall(CODE)\
 auto fn = [=]{CODE};\
	if (Thread::IsMainThread())\
	{\
		Graphics::AddCommand(fn);\
	}\
	else\
	{\
		fn();\
	}
}

