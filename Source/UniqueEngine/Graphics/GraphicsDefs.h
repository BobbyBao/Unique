#pragma once

#include "Container/Container.h"
#include "Serialize/SerializeTraits.h"
#include <GraphicsTypes.h>
#include <DeviceContext.h>

using namespace Diligent;

namespace Unique
{
	
	enum class RendererID
	{
		Direct3D11,
		Direct3D12,
		OpenGL,
		OpenGLES,
		Vulkan,
		Metal
	};
	
	using PrimitiveTopology = PRIMITIVE_TOPOLOGY;
	
	uEnumTraits(PrimitiveTopology,
				"UNDEFINED", "TRIANGLE_LIST", "TRIANGLE_STRIP", "POINT_LIST", "LINE_LIST", 
				"PATCHES_1", "PATCHES_2", "PATCHES_3", "PATCHES_4", "PATCHES5", "PATCHES6", "PATCHES7", "PATCHES8",
				"PATCHES_9", "PATCHES_10", "PATCHES_11", "PATCHES_12", "PATCHES_13", "PATCHES_14", "PATCHES_16", "PATCHES_16",
				"PATCHES_17", "PATCHES_18", "PATCHES_19", "PATCHES_20", "PATCHES_21", "PATCHES_22", "PATCHES_23", "PATCHES_24",
				"PATCHES_25", "PATCHES_26", "PATCHES_27", "PATCHES_28", "PATCHES_29", "PATCHES_30", "PATCHES_31", "PATCHES_32")

#if false
	using ShaderStageFlags = LLGL::ShaderStageFlags;
	using ClearFlags = LLGL::ClearFlags;

	using Window = LLGL::Window;
	using Viewport = LLGL::Viewport;
	using Scissor = LLGL::Scissor;
	using RenderTarget = LLGL::RenderTarget;

	using CompareOp = LLGL::CompareOp;
	using DepthState = LLGL::DepthDescriptor;
	using StencilState = LLGL::StencilDescriptor;
	using RasterizerState = LLGL::RasterizerDescriptor;
	using BlendState = LLGL::BlendDescriptor;

	using GraphicsPipelineDescriptor = LLGL::GraphicsPipelineDescriptor;

	using GraphicsPipeline = LLGL::GraphicsPipeline;
	using ComputePipeline = LLGL::ComputePipeline;

	using VertexFormat = LLGL::VertexFormat;
	using StreamOutputFormat = LLGL::StreamOutputFormat;
	using ShaderType = LLGL::ShaderType;
	using ShaderProgram = LLGL::ShaderProgram;
	using PrimitiveType = LLGL::PrimitiveTopology;

	using UniformType = LLGL::UniformType;

	uEnumTraits(ShaderType, "VertexShader", "HullShader", "DomainShader", "GeometryShader",	"FragmentShader", "ComputeShader")
	uEnumTraits(CompareOp, "Never", "Less", "Equal", "LessEqual", "Greater", "NotEqual", "GreaterEqual", "Ever")
	uClassTraits(DepthState, "TestEnabled", self.testEnabled, "WriteEnabled", self.writeEnabled, "CompareOp", self.compareOp)

	uEnumTraits(UniformType, "Float", "Float2", "Float3", "Float4", 
							 "Double", "Double2", "Double3", "Double4",
							 "Int", "Int2", "Int3", "Int4", )

#endif
}