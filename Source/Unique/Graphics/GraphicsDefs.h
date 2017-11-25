#pragma once
#include <LLGL/LLGL.h>
#include "../Container/Container.h"
#include "../Serialize/SerializeTraits.h"

using namespace LLGL;

namespace Unique
{

	extern UPtr<RenderSystem>		renderer;
	extern LLGL::RenderContext*		graphicsContext;
	extern LLGL::CommandBuffer*		commands;

	template <typename VertexType>
	Buffer* CreateVertexBuffer(const std::vector<VertexType>& vertices, const LLGL::VertexFormat& vertexFormat)
	{
		return renderer->CreateBuffer(
			VertexBufferDesc(static_cast<unsigned int>(vertices.size() * sizeof(VertexType)), vertexFormat),
			vertices.data()
		);
	}

	template <typename IndexType>
	Buffer* CreateIndexBuffer(const std::vector<IndexType>& indices, const LLGL::IndexFormat& indexFormat)
	{
		return renderer->CreateBuffer(
			LLGL::IndexBufferDesc(static_cast<unsigned int>(indices.size() * sizeof(IndexType)), indexFormat),
			indices.data()
		);
	}

	template <typename Buffer>
	Buffer* CreateConstantBuffer(const Buffer& buffer)
	{
		static_assert(!std::is_pointer<Buffer>::value, "buffer type must not be a pointer");
		return renderer->CreateBuffer(
			LLGL::ConstantBufferDesc(sizeof(buffer)),
			&buffer
		);
	}

	template <typename T>
	void UpdateBuffer(LLGL::Buffer* buffer, const T& data)
	{
		GS_ASSERT(buffer != nullptr);
		renderer->WriteBuffer(*buffer, &data, sizeof(data), 0);
	}

	// Returns the aspect ratio of the render context resolution (X:Y).
	inline float GetAspectRatio()
	{
		auto resolution = graphicsContext->GetVideoMode().resolution.Cast<float>();
		return (resolution.x / resolution.y);
	}

	// Returns ture if OpenGL is used as rendering API.
	inline bool IsOpenGL()
	{
		return (renderer->GetRendererID() == LLGL::RendererID::OpenGL);
	}


	inline Gs::Matrix4f PerspectiveProjection(float aspectRatio, float near, float far, float fov)
	{
		int flags = (IsOpenGL() ? Gs::ProjectionFlags::UnitCube : 0);
		return Gs::ProjectionMatrix4f::Perspective(aspectRatio, near, far, fov, flags).ToMatrix4();
	}




	template<>
	class SerializeTraits<LLGL::ShaderType> : public SerializeTraitsEnum<LLGL::ShaderType>
	{
	public:
		typedef LLGL::ShaderType value_type;

		template<class TransferFunction> 
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			static const char* shaderType[] =
			{
				"VertexShader", "HullShader", "DomainShader", "GeometryShader",
				"FragmentShader", "ComputeShader"
			};

			TransferEnum<TransferFunction>(data, shaderType, 6, transfer);
		}

	};
}