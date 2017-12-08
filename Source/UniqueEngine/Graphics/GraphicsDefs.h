#pragma once

#include "Container/Container.h"
#include "Serialize/SerializeTraits.h"
#include <GraphicsTypes.h>
#include <DeviceContext.h>

using namespace Diligent;

namespace Unique
{
	using ValueType = VALUE_TYPE;
	using BindFlags = BIND_FLAGS;
	using PrimitiveTopology = PRIMITIVE_TOPOLOGY;
	using Usage = USAGE;
	using CPUAccessFlag = CPU_ACCESS_FLAG;
	using MapType = MAP_TYPE;
	using MapFlags = MAP_FLAGS;

	using TextureFormat = TEXTURE_FORMAT;
	using FilterType = FILTER_TYPE;
	using TextureAddressMode = TEXTURE_ADDRESS_MODE;
	using ComponentType = COMPONENT_TYPE;

	using StencilOp = STENCIL_OP;
	using ComparisonFunction = COMPARISON_FUNCTION;
	using FillMode = FILL_MODE;
	using CullMode = CULL_MODE;

	using BlendFactor = BLEND_FACTOR;
	using BlendOperation = BLEND_OPERATION;
	using LogicOperation = LOGIC_OPERATION;
	using ColorMask = COLOR_MASK;

	uEnumTraits(ValueType,
				"UNDEFINED", "INT8", "INT16", "INT32", "UINT8", "UINT16", "UINT32", "FLOAT16", "FLOAT32")
	
	uEnumTraits(PrimitiveTopology,
				"UNDEFINED", "TRIANGLE_LIST", "TRIANGLE_STRIP", "POINT_LIST", "LINE_LIST", 
				"PATCHES_1", "PATCHES_2", "PATCHES_3", "PATCHES_4", "PATCHES5", "PATCHES6", "PATCHES7", "PATCHES8",
				"PATCHES_9", "PATCHES_10", "PATCHES_11", "PATCHES_12", "PATCHES_13", "PATCHES_14", "PATCHES_16", "PATCHES_16",
				"PATCHES_17", "PATCHES_18", "PATCHES_19", "PATCHES_20", "PATCHES_21", "PATCHES_22", "PATCHES_23", "PATCHES_24",
				"PATCHES_25", "PATCHES_26", "PATCHES_27", "PATCHES_28", "PATCHES_29", "PATCHES_30", "PATCHES_31", "PATCHES_32")

	uEnumTraits(StencilOp, "UNDEFINED", "KEEP", "ZERO", "REPLACE", 
		"INCR_SAT", "DECR_SAT", "INVERT", "INCR_WRAP", "DECR_WRAP")

	uEnumTraits(ComparisonFunction, "UNKNOWN", "NEVER", "LESS", "EQUAL", 
		"LESS_EQUAL", "GREATER", "NOT_EQUAL", "GREATER_EQUAL", "ALWAYS")

	uEnumTraits(FillMode, "UNDEFINED", "WIREFRAME", "SOLID")

	uEnumTraits(CullMode, "UNDEFINED", "NONE", "FRONT", "BACK")

	uEnumTraits(BlendFactor, "UNDEFINED", "ZERO", "ONE", "SRC_COLOR", "INV_SRC_COLOR", 
		"SRC_ALPHA", "INV_SRC_ALPHA", "DEST_ALPHA", "INV_DEST_ALPHA", "DEST_COLOR",
		"INV_DEST_COLOR", "SRC_ALPHA_SAT", "BLEND_FACTOR", "INV_BLEND_FACTOR", "SRC1_COLOR",
		"INV_SRC1_COLOR", "SRC1_ALPHA", "INV_SRC1_ALPHA")

	uEnumTraits(BlendOperation, "UNDEFINED", "ADD", "SUBTRACT", "REV_SUBTRACT", "MIN", "MAX")
		
	uEnumTraits(LogicOperation, "CLEAR", "SET", "COPY", "COPY_INVERTED", "NOOP", 
		"INVERT", "AND", "NAND", "OR", "NOR", "XOR", "EQUIV", "AND_REVERSE",
		"AND_INVERTED",	"OR_REVERSE", "OR_INVERTED")
		
	uFlagsTraits(ColorMask, {"RED", COLOR_MASK_RED}, {"GREEN", COLOR_MASK_GREEN}, {"BLUE", COLOR_MASK_BLUE}, {"ALPHA", COLOR_MASK_ALPHA})
		
}