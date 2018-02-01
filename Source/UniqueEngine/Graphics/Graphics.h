#pragma once
#include "GraphicsDefs.h"
#include "Core/Object.h"
#include "Core/Semaphore.h"
#include "GraphicsEvents.h"
#include "GraphicsBuffer.h"

struct SDL_Window;

namespace Unique
{
	class VertexBuffer;
	class IndexBuffer;
	class Geometry;
	class Texture;
	class TextureView;
	class GPUObject;
	class ShaderVariation;
	class Sampler;
	class PipelineState;
	
	using CommandQueue = Vector<std::function<void()> > ;
	
	class Graphics : public Object
	{
		uRTTI(Graphics, Object)
	public:
		Graphics();
		~Graphics();
		
		bool CreateWindow(const IntVector2& size, DeviceType deviceType = DeviceType::D3D11);		
		bool Initialize();
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
		bool IsSRgb() const { return srgb_; }

		//***MainThread***
		template<class T>
		SPtr<UniformBuffer> AddUniform()
		{
			SPtr<UniformBuffer> uniformBuffer(new UniformBuffer(T()));
			AddResource(TypeInfo::GetTypeName<T>(), uniformBuffer);
			return uniformBuffer;
		}

		void AddResource(const char *Name, GPUObject* pObject, bool bIsUnique = true);
		void RemoveResourceByName(const char *Name, uint ArrayIndex = 0);
		void Frame();
		//****************

		//*******Render thread*******	
		void CreateBuffer(GraphicsBuffer& buffer, const ByteArray& data);
		void CreateShader(ShaderVariation& shader);
		void CreateSampler(Sampler& sampler);
		void CreateTexture(Texture& texture);
		void CreatePipelineState(PipelineState& pipelineState);
		void ReleaseDeviceObject(void* deviceObject);

		void* Map(GraphicsBuffer* buffer, MapFlags mapFlags = MapFlags::DISCARD);
		void Unmap(GraphicsBuffer* buffer, MapFlags mapFlags = MapFlags::DISCARD);
		
		bool BeginRender();
		void EndRender();    
		void Clear(TextureView *pView, const Color& color, uint ClearFlags = CLEAR_COLOR, float fDepth = 1.0f, byte Stencil = 0xff);
		void SetScissorRects(uint NumRects, const IntRect *pRects, uint RTWidth = 0, uint RTHeight = 0);
		void Draw(Geometry* geometry, PipelineState* pipeline);
		void Draw(Geometry* geometry, PipelineState* pipeline, PrimitiveTopology primitiveType, 
		unsigned vertexStart, unsigned vertexCount, unsigned indexStart, unsigned indexCount);
		void DrawInstanced(Geometry* geometry, PipelineState* pipeline, uint instanceOffset, uint numInstances);

		void Close();

		//**************************
		static void AddCommand(const std::function<void()>& cmd);
		static void PostCommand(const std::function<void()>& cmd);
		static void FrameNoRenderWait();
		static bool IsRenderThread();
		static int currentContext_;
		static int currentFrame_;
		inline static int GetRenderContext() { return 1 - currentContext_; }

		static float GetRenderWait() { return  waitSubmit_/ 1000.0f;}
		static float GetUpdateWait() { return  waitRender_/ 1000.0f;}
	protected:
		static void SetRenderThread();
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
		int multiSampling_ = 4;
		bool srgb_ = false;

		struct Impl& impl_;

		bool inited_ = false;
		SDL_Window *window_ = nullptr;
		static ThreadID renderThreadID;
		static bool singleThreaded_;
		static Semaphore renderSem_;
		static Semaphore mainSem_;
		static long long waitSubmit_;
		static long long waitRender_;
		static CommandQueue comands_;
		static CommandQueue postComands_;

		template<class T>
		friend T& MainContext(T* data);
		template<class T>
		friend T& RenderContext(T* data);
	};

	template<class T>
	inline T& MainContext(T* data)
	{
		assert(Thread::IsMainThread());
		return data[Graphics::currentContext_];
	}

	template<class T>
	inline T& RenderContext(T* data)
	{
		assert(Graphics::IsRenderThread());
		return data[1 - Graphics::currentContext_];
	}

#ifdef DEBUG_RENDER
	#define LOG_RENDER(...)     LOG_DEBUG_MESSAGE(BasicPlatformDebug::DebugMessageSeverity::Info,    ##__VA_ARGS__)
#else
	#define LOG_RENDER(...)
#endif


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

#define uPost(CODE)\
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