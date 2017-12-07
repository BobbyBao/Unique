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
		
		bool Initialize(const std::string& rendererModule, const IntVector2& size);
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

		/*
		inline Gs::Matrix4f PerspectiveProjection(float aspectRatio, float near, float far, float fov)
		{
			int flags = (IsOpenGL() ? Gs::ProjectionFlags::UnitCube : 0);
			return Gs::ProjectionMatrix4f::Perspective(aspectRatio, near, far, fov, flags).ToMatrix4();
		}*/
			
		//***MainThread***
		void BeginFrame();
		void EndFrame();
		//****************

		//Execute in render thread
		void BeginRender();
		void EndRender();
		void Clear(uint flags);
		/*
		void SetRenderTarget(RenderTarget* renderTarget);
		void SetViewport(const Viewport& viewport);
		void SetScissor(const Scissor& scissor);
		void SetGraphicsPipeline(GraphicsPipeline* graphicsPipeline);
        void SetComputePipeline(ComputePipeline* computePipeline);
		void SetVertexBuffer(VertexBuffer* buffer);
		void SetVertexBuffers(BufferArray* buffers);
		void SetIndexBuffer(IndexBuffer* buffer);
		void SetTexture(Texture* texture, uint slot, long shaderStageFlags = ShaderStageFlags::AllStages);
		*/
        void Draw(unsigned int numVertices, unsigned int firstVertex);
        void DrawIndexed(unsigned int numIndexes, unsigned int firstIndex);
        void DrawIndexed(unsigned int numIndexes, unsigned int firstIndex, int vertexOffset);
        void DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances);
        void DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances, unsigned int instanceOffset);
        void DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex);
        void DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset);
        void DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset, unsigned int instanceOffset);
        void Dispatch(unsigned int groupSizeX, unsigned int groupSizeY, unsigned int groupSizeZ);
		void Close();
	protected:
		RefCntAutoPtr<IRenderDevice> pRenderDevice;
		RefCntAutoPtr<IDeviceContext> pDeviceContext;
		RefCntAutoPtr<ISwapChain> pSwapChain;

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

