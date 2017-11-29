#pragma once
#include <LLGL/LLGL.h>
#include <LLGL/Utility.h>
#include <LLGL/GraphicsPipelineFlags.h>
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

	using CompareOp = LLGL::CompareOp;
	using DepthState = LLGL::DepthDescriptor;
	using StencilState = LLGL::StencilDescriptor;
	using RasterizerState = LLGL::RasterizerDescriptor;
	using BlendState = LLGL::BlendDescriptor;

	using GraphicsPipelineDescriptor = LLGL::GraphicsPipelineDescriptor;

	using GraphicsPipeline = LLGL::GraphicsPipeline;
	using ComputePipeline = LLGL::ComputePipeline;

	using VertexFormat = LLGL::VertexFormat;
	using IndexFormat = LLGL::IndexFormat;
	using StreamOutputFormat = LLGL::StreamOutputFormat;
	using ShaderType = LLGL::ShaderType;
	using ShaderProgram = LLGL::ShaderProgram;
	using PrimitiveTopology = LLGL::PrimitiveTopology;

	uEnumTraits(PrimitiveTopology,
				"PointList", "LineList", "LineStrip", "LineLoop", "LineListAdjacency", "LineStripAdjacency",
				"TriangleList", "TriangleStrip", "TriangleFan", "TriangleListAdjacency", "TriangleStripAdjacency",
				"Patches1", "Patches2", "Patches3", "Patches4", "Patches5", "Patches6", "Patches7", "Patches8",
				"Patches9", "Patches10", "Patches11", "Patches12", "Patches13", "Patches14", "Patches16", "Patches16",
				"Patches17", "Patches18", "Patches19", "Patches20", "Patches21", "Patches22", "Patches23", "Patches24",
				"Patches25", "Patches26", "Patches27", "Patches28", "Patches29", "Patches30", "Patches31", "Patches32")
	
	uEnumTraits(ShaderType, "VertexShader", "HullShader", "DomainShader", "GeometryShader",	"FragmentShader", "ComputeShader")
	uEnumTraits(CompareOp, "Never", "Less", "Equal", "LessEqual", "Greater", "NotEqual", "GreaterEqual", "Ever")
	uClassTraits(DepthState, "TestEnabled", self.testEnabled, "WriteEnabled", self.writeEnabled, "CompareOp", self.compareOp)

}