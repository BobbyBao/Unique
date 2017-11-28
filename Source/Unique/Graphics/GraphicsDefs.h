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

	using VertexFormat = LLGL::VertexFormat;
	using IndexFormat = LLGL::IndexFormat;
	using StreamOutputFormat = LLGL::StreamOutputFormat;
	using ShaderType = LLGL::ShaderType;
	using ShaderProgram = LLGL::ShaderProgram;
	using PrimitiveTopology = LLGL::PrimitiveTopology;

	template<>
	class SerializeTraits<PrimitiveTopology> : public SerializeTraitsEnum<PrimitiveTopology>
	{
	public:
		typedef PrimitiveTopology value_type;

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			static const char* primitiveTopology[] =
			{
				"PointList", "LineList", "LineStrip", "LineLoop", "LineListAdjacency", "LineStripAdjacency",
				"TriangleList", "TriangleStrip", "TriangleFan", "TriangleListAdjacency", "TriangleStripAdjacency",
				"Patches1", "Patches2", "Patches3", "Patches4", "Patches5", "Patches6", "Patches7", "Patches8",
				"Patches9", "Patches10", "Patches11", "Patches12", "Patches13", "Patches14", "Patches16", "Patches16",
				"Patches17", "Patches18", "Patches19", "Patches20", "Patches21", "Patches22", "Patches23", "Patches24",
				"Patches25", "Patches26", "Patches27", "Patches28", "Patches29", "Patches30", "Patches31", "Patches32"
			};

			TransferEnum<TransferFunction>(data, primitiveTopology, transfer);
		}

	};

	template<>
	class SerializeTraits<ShaderType> : public SerializeTraitsEnum<ShaderType>
	{
	public:
		typedef ShaderType value_type;

		template<class TransferFunction> 
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			static const char* shaderType[] =
			{
				"VertexShader", "HullShader", "DomainShader", "GeometryShader",
				"FragmentShader", "ComputeShader",
			};

			TransferEnum<TransferFunction>(data, shaderType, transfer);
		}

	};
}