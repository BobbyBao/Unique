#pragma once
#include "GraphicsDefs.h"
#include <DepthStencilState.h>

#include <BlendState.h>

namespace Unique
{
	using FilterType = Diligent::FILTER_TYPE;
	using TextureAddressMode = Diligent::TEXTURE_ADDRESS_MODE;
	using ComparisonFunc = Diligent::COMPARISON_FUNCTION;
	using StencilOp = Diligent::STENCIL_OP;
	using BlendFactor = Diligent::BLEND_FACTOR;
	using BlendOperation = Diligent::BLEND_OPERATION;
	using LogicOperation = Diligent::LOGIC_OPERATION;
	using ColorMask = Diligent::COLOR_MASK;

	using RasterizerState = Diligent::RasterizerStateDesc;
	using StencilOpDesc = Diligent::StencilOpDesc;
	using DepthStencilState = Diligent::DepthStencilStateDesc;
	using RenderTargetBlendDesc = Diligent::RenderTargetBlendDesc;
	using BlendState = Diligent::BlendStateDesc;


	uEnum(StencilOp, "UNDEFINED", "KEEP", "ZERO", "REPLACE",
		"INCR_SAT", "DECR_SAT", "INVERT", "INCR_WRAP", "DECR_WRAP")

		uEnum(ComparisonFunc, "UNKNOWN", "NEVER", "LESS", "EQUAL",
			"LESS_EQUAL", "GREATER", "NOT_EQUAL", "GREATER_EQUAL", "ALWAYS")

		uEnum(FillMode, "UNDEFINED", "WIREFRAME", "SOLID")

		uEnum(CullMode, "UNDEFINED", "NONE", "FRONT", "BACK")

		uEnum(BlendFactor, "UNDEFINED", "ZERO", "ONE", "SRC_COLOR", "INV_SRC_COLOR",
			"SRC_ALPHA", "INV_SRC_ALPHA", "DEST_ALPHA", "INV_DEST_ALPHA", "DEST_COLOR",
			"INV_DEST_COLOR", "SRC_ALPHA_SAT", "BLEND_FACTOR", "INV_BLEND_FACTOR", "SRC1_COLOR",
			"INV_SRC1_COLOR", "SRC1_ALPHA", "INV_SRC1_ALPHA")

		uEnum(BlendOperation, "UNDEFINED", "ADD", "SUBTRACT", "REV_SUBTRACT", "MIN", "MAX")

		uEnum(LogicOperation, "CLEAR", "SET", "COPY", "COPY_INVERTED", "NOOP",
			"INVERT", "AND", "NAND", "OR", "NOR", "XOR", "EQUIV", "AND_REVERSE",
			"AND_INVERTED", "OR_REVERSE", "OR_INVERTED")


		uFlags(ColorMask, { "RED", COLOR_MASK_RED }, { "GREEN", COLOR_MASK_GREEN }, { "BLUE", COLOR_MASK_BLUE }, { "ALPHA", COLOR_MASK_ALPHA }, { "ALL", COLOR_MASK_ALL })


	
	uClassTraits
	(
		RasterizerState,
		"FillMode", self.FillMode,
		"CullMode", self.CullMode,
		"FrontCounterClockwise", self.FrontCounterClockwise,
		"DepthBias", self.DepthBias,
		"DepthBiasClamp", self.DepthBiasClamp,
		"SlopeScaledDepthBias", self.SlopeScaledDepthBias,
		"DepthClipEnable", self.DepthClipEnable,
		"ScissorEnable", self.ScissorEnable,
		"AntialiasedLineEnable", self.AntialiasedLineEnable
	);

	uClassTraits
	(
		StencilOpDesc,
		"StencilFailOp", self.StencilFailOp,
		"StencilDepthFailOp", self.StencilDepthFailOp,
		"StencilPassOp", self.StencilPassOp,
		"StencilFunc", self.StencilFunc
	);

	uClassTraits
	(
		DepthStencilState,
		"DepthEnable", self.DepthEnable,
		"DepthWriteEnable", self.DepthWriteEnable,
		"ComparisonFunction", self.DepthFunc,
		"StencilEnable", self.StencilEnable,
		"StencilReadMask", self.StencilReadMask,
		"StencilWriteMask", self.StencilWriteMask,
		"FrontFace", self.FrontFace,
		"BackFace", self.BackFace
	);

}