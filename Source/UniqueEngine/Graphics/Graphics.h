#pragma once
#include "GraphicsDefs.h"
#include "Core/Object.h"
#include <RefCntAutoPtr.h>
#include <RenderDevice.h>
#include <SwapChain.h>


namespace Unique
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;

	class Graphics : public Object
	{
		uRTTI(Graphics, Object)
	public:
		Graphics();
		~Graphics();
		
		bool Initialize(const IntVector2& size, DeviceType deviceType = DeviceType::D3D11);
		void Resize(const IntVector2& size);

		DeviceType GetDeviceType() const;
		void SetDebug(bool val);
		const IntVector2& GetResolution() const;

		// Returns the aspect ratio of the render context resolution (X:Y).
		inline float GetAspectRatio() const
		{
			return GetResolution().x_ / (float)GetResolution().y_;
		}

		bool IsDirect3D() const;
		bool IsOpenGL() const;

		//***MainThread***
		void BeginFrame();
		void EndFrame();
		//****************

		//Execute in render thread
		void BeginRender();
		void EndRender();
		void Close();
	protected:
		RefCntAutoPtr<IRenderDevice> renderDevice_;
		RefCntAutoPtr<IDeviceContext> deviceContext_;
		RefCntAutoPtr<ISwapChain> swapChain_;

		IntVector2 resolution_;
		DeviceType deviceType_;
		bool debugger_ = false;

		bool vsync_ = false;

		int multiSampling_ = 4;


		bool exit_ = false;

	};
	
	extern IRenderDevice* renderDevice;
	extern IDeviceContext* deviceContext;
}

