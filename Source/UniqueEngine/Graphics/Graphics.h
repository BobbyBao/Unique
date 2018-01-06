#pragma once
#include "GraphicsDefs.h"
#include "Core/Object.h"
#include "Core/Semaphore.h"
#include "GraphicsEvents.h"
#include "GraphicsBuffer.h"
#include <RefCntAutoPtr.h>
#include <RenderDevice.h>
#include <SwapChain.h>

struct SDL_Window;

namespace Unique
{
	using IRenderDevice = Diligent::IRenderDevice;
	using IDeviceContext = Diligent::IDeviceContext;
	using ISwapChain = Diligent::ISwapChain;

	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class GPUObject;
	class ShaderVariation;
	class PipelineState;
	
	using CommandQueue = Vector<std::function<void()> > ;

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
		int GetWidth() const { return resolution_.x_; }
		int GetHeight() const { return resolution_.y_; }
		const IntVector2& GetResolution() const;

		// Returns the aspect ratio of the render context resolution (X:Y).
		inline float GetAspectRatio() const
		{
			return GetResolution().x_ / (float)GetResolution().y_;
		}

		SDL_Window * GetWindow() { return window_; }

		bool IsDirect3D() const;
		bool IsOpenGL() const;

		//***MainThread***

		template<class T>
		SPtr<UniformBuffer> AddUniformBuffer()
		{
			SPtr<UniformBuffer> uniformBuffer(new UniformBuffer(T()));
			AddResource(TypeInfo::GetTypeName<T>(), uniformBuffer);
			return uniformBuffer;
		}

		void AddResource(const char *Name, GPUObject* pObject, bool bIsUnique = true);
		void AddResource(const char *Name, IDeviceObject *pObject, bool bIsUnique);   
		void AddResourceArray(const char *Name, uint StartIndex, IDeviceObject* const* ppObjects, uint NumElements, bool bIsUnique);
		void RemoveResourceByName(const char *Name, uint ArrayIndex = 0);
		void BindShaderResources(IPipelineState* pipelineState, uint flags);
		void BindResources(IShaderResourceBinding* shaderResourceBinding, uint shaderFlags, uint flags);
		void Frame();
		//****************

		//*******Render thread*******
		virtual void CreateBuffer(const Diligent::BufferDesc& buffDesc, const Diligent::BufferData& buffData, GraphicsBuffer& buffer);
		virtual void CreateShader(const Diligent::ShaderCreationAttribs &creationAttribs, Diligent::IShader** shader);
		virtual void CreateTexture(const Diligent::TextureDesc& texDesc, const Diligent::TextureData &data,	Texture& texture);
		virtual void CreateSampler(const Diligent::SamplerDesc& samDesc, Diligent::ISampler **ppSampler);
		virtual void CreateResourceMapping(const Diligent::ResourceMappingDesc &mappingDesc, Diligent::IResourceMapping **ppMapping);
		virtual void CreatePipelineState(const Diligent::PipelineStateDesc &pipelineDesc, IPipelineState** pipelineState);



		void BeginRender();
		void EndRender();
		void Close();
		//**************************
		static int currentContext_;
		inline static int GetRenderContext() { return 1 - currentContext_; }
		static void AddCommand(const std::function<void()>& cmd);
		static void FrameNoRenderWait();
	protected:
		static void ExecuteCommands(CommandQueue& cmds);
		static void MainSemPost();
		static bool MainSemWait(int _msecs = -1);
		static void SwapContext();
		static void RenderSemPost();
		static void RenderSemWait();

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

		SDL_Window *window_ = nullptr;

		static bool singleThreaded_;
		static Semaphore renderSem_;
		static Semaphore mainSem_;
		static long long waitSubmit_;
		static long long waitRender_;
		static CommandQueue comands_;
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

