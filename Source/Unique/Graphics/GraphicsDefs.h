#pragma once
#include <LLGL/LLGL.h>
#include <LLGL/Utility.h>
#include "../Container/Container.h"
#include "../Serialize/SerializeTraits.h"

namespace Unique
{
	using ShaderStageFlags = LLGL::ShaderStageFlags;
	using ClearFlags = LLGL::ClearFlags;

	using Window = LLGL::Window;
	using Viewport = LLGL::Viewport;
	using Scissor = LLGL::Scissor;
	using RenderTarget = LLGL::RenderTarget;
	using GraphicsPipeline = LLGL::GraphicsPipeline;
	using ComputePipeline = LLGL::ComputePipeline;

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