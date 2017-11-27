#pragma once
#include "GraphicsDefs.h"
#include "../Core/Object.h"
#include "../Core/Semaphore.h"

namespace Unique
{
	class VertexBuffer;
	class IndexBuffer;
	class Texture;

	using CommandQueue = Vector<std::function<void()>>;

	class Graphics : public Object
	{
		uRTTI(Graphics, Object)
	public:
		Graphics();
		~Graphics();
		
		Window* Initialize(const std::string& rendererModule, const LLGL::Size& size);
		void Resize(const LLGL::Size& size);

		const std::string GetRenderName() const;
		void SetDebug(bool val);
		const Size& GetResolution() const;

		// Returns the aspect ratio of the render context resolution (X:Y).
		inline float GetAspectRatio() const
		{
			auto resolution = GetResolution().Cast<float>();
			return (resolution.x / resolution.y);
		}

		bool IsOpenGL() const;


		inline Gs::Matrix4f PerspectiveProjection(float aspectRatio, float near, float far, float fov)
		{
			int flags = (IsOpenGL() ? Gs::ProjectionFlags::UnitCube : 0);
			return Gs::ProjectionMatrix4f::Perspective(aspectRatio, near, far, fov, flags).ToMatrix4();
		}

		SPtr<VertexBuffer> CreateVertexBuffer(uint size, const LLGL::VertexFormat& vertexFormat, void* data = nullptr);
		SPtr<IndexBuffer> CreateIndexBuffer(uint size, const LLGL::IndexFormat& indexFormat, void* data = nullptr);

	
		//***MainThread***
		void BeginFrame();
		void EndFrame();
		void AddCommand(std::function<void()> cmd);
		void PostCommand(std::function<void()> cmd);
		//****************

		//Execute in render thread
		void BeginRender();
		void EndRender();
		void Clear(uint flags);
		void SetRenderTarget(RenderTarget* renderTarget);
		void SetViewport(const Viewport& viewport);
		void SetScissor(const Scissor& scissor);
		void SetGraphicsPipeline(GraphicsPipeline* graphicsPipeline);
        void SetComputePipeline(ComputePipeline* computePipeline);
		void SetVertexBuffer(VertexBuffer* buffer);
		void SetIndexBuffer(IndexBuffer* buffer);
		void SetTexture(Texture* texture, uint slot, long shaderStageFlags = ShaderStageFlags::AllStages);

		/**
        \brief Draws the specified amount of primitives from the currently set vertex buffer.
        \param[in] numVertices Specifies the number of vertices to generate.
        \param[in] firstVertex Specifies the zero-based offset of the first vertex from the vertex buffer.
        */
        void Draw(unsigned int numVertices, unsigned int firstVertex);

        //! \see DrawIndexed(unsigned int, unsigned int, int)
        void DrawIndexed(unsigned int numVertices, unsigned int firstIndex);

        /**
        \brief Draws the specified amount of primitives from the currently set vertex- and index buffers.
        \param[in] numVertices Specifies the number of vertices to generate.
        \param[in] firstIndex Specifies the zero-based offset of the first index from the index buffer.
        \param[in] vertexOffset Specifies the base vertex offset (positive or negative) which is added to each index from the index buffer.
        */
        void DrawIndexed(unsigned int numVertices, unsigned int firstIndex, int vertexOffset);

        //! \see DrawInstanced(unsigned int, unsigned int, unsigned int, unsigned int)
        void DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances);
        
        /**
        \brief Draws the specified amount of instances of primitives from the currently set vertex buffer.
        \param[in] numVertices Specifies the number of vertices to generate.
        \param[in] firstVertex Specifies the zero-based offset of the first vertex from the vertex buffer.
        \param[in] numInstances Specifies the number of instances to generate.
        \param[in] instanceOffset Specifies the zero-based instance offset which is added to each instance ID.
        */
        void DrawInstanced(unsigned int numVertices, unsigned int firstVertex, unsigned int numInstances, unsigned int instanceOffset);

        //! \see DrawIndexedInstanced(unsigned int, unsigned int, unsigned int, int, unsigned int)
        void DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex);
        
        //! \see DrawIndexedInstanced(unsigned int, unsigned int, unsigned int, int, unsigned int)
        void DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset);
        
        /**
        \brief Draws the specified amount of instances of primitives from the currently set vertex- and index buffers.
        \param[in] numVertices Specifies the number of vertices to generate.
        \param[in] numInstances Specifies the number of instances to generate.
        \param[in] firstIndex Specifies the zero-based offset of the first index from the index buffer.
        \param[in] vertexOffset Specifies the base vertex offset (positive or negative) which is added to each index from the index buffer.
        \param[in] instanceOffset Specifies the zero-based instance offset which is added to each instance ID.
        */
        void DrawIndexedInstanced(unsigned int numVertices, unsigned int numInstances, unsigned int firstIndex, int vertexOffset, unsigned int instanceOffset);

        /* ----- Compute ----- */

        /**
        \brief Dispachtes a compute command.
        \param[in] groupSizeX Specifies the number of thread groups in the X-dimension.
        \param[in] groupSizeY Specifies the number of thread groups in the Y-dimension.
        \param[in] groupSizeZ Specifies the number of thread groups in the Z-dimension.
        \see SetComputePipeline
        \see RenderingCaps::maxNumComputeShaderWorkGroups
        */
        void Dispatch(unsigned int groupSizeX, unsigned int groupSizeY, unsigned int groupSizeZ);

		void Close();
	protected:
		void ExecuteCommands(CommandQueue& cmds);
		void FrameNoRenderWait();
		void MainSemPost();
		bool MainSemWait(int _msecs = -1);
		void SwapContext();
		void RenderSemPost();
		void RenderSemWait();

		bool debugger_ = false;
		bool vsync_ = false;
		int multiSampling_ = 4;
		UPtr<LLGL::RenderingProfiler>    profilerObj_;
		UPtr<LLGL::RenderingDebugger>    debuggerObj_;
		Map<LLGL::SamplerDescriptor, LLGL::Sampler*> samplers_;

		Semaphore renderSem_;
		Semaphore mainSem_;
		bool singleThreaded_ = false;
		bool exit_ = false;

		Vector < std::function<void()>> preComands_;
		Vector < std::function<void()>> postComands_;

		long long waitSubmit_ = 0;
		long long waitRender_ = 0;
	};

}

