#pragma once

#include "Container/Container.h"
#include "Serialize/SerializeTraits.h"
#include <GraphicsTypes.h>
#include <DeviceContext.h>

namespace Unique
{
	using DeviceType = Diligent::DeviceType;
	using ValueType = Diligent::VALUE_TYPE;
	using BindFlags = Diligent::BIND_FLAGS;
	using Usage = Diligent::USAGE;
	using CPUAccessFlag = Diligent::CPU_ACCESS_FLAG;
	using MapType = Diligent::MAP_TYPE;
	using MapFlags = Diligent::MAP_FLAGS;

	using TextureFormat = Diligent::TEXTURE_FORMAT;
	using FilterType = Diligent::FILTER_TYPE;
	using TextureAddressMode = Diligent::TEXTURE_ADDRESS_MODE;
	using ComponentType = Diligent::COMPONENT_TYPE;

	using StencilOp = Diligent::STENCIL_OP;
	using ComparisonFunction = Diligent::COMPARISON_FUNCTION;
	using FillMode = Diligent::FILL_MODE;
	using CullMode = Diligent::CULL_MODE;

	using BlendFactor = Diligent::BLEND_FACTOR;
	using BlendOperation = Diligent::BLEND_OPERATION;
	using LogicOperation = Diligent::LOGIC_OPERATION;
	using ColorMask = Diligent::COLOR_MASK;
	
	using IDeviceObject = Diligent::IDeviceObject;
	using IPipelineState = Diligent::IPipelineState;
	using IShaderResourceBinding = Diligent::IShaderResourceBinding;
	using IResourceMapping = Diligent::IResourceMapping;

	enum class PrimitiveTopology : byte
	{
		/// Undefined topology
		UNDEFINED = 0,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
		POINT_LIST,
		LINE_LIST,
		PATCHLIST_1,
		PATCHLIST_2,
		PATCHLIST_3,
		PATCHLIST_4,
		PATCHLIST_5,
		PATCHLIST_6,
		PATCHLIST_7,
		PATCHLIST_8,
		PATCHLIST_9,
		PATCHLIST_10,
		PATCHLIST_11,
		PATCHLIST_12,
		PATCHLIST_13,
		PATCHLIST_14,
		PATCHLIST_15,
		PATCHLIST_16,
		PATCHLIST_17,
		PATCHLIST_18,
		PATCHLIST_19,
		PATCHLIST_20,
		PATCHLIST_21,
		PATCHLIST_22,
		PATCHLIST_23,
		PATCHLIST_24,
		PATCHLIST_25,
		PATCHLIST_26,
		PATCHLIST_27,
		PATCHLIST_28,
		PATCHLIST_29,
		PATCHLIST_30,
		PATCHLIST_31,
		PATCHLIST_32,
		NUM_TOPOLOGIES
	};

	/// Arbitrary vertex declaration element datatypes.
	enum VertexElementType
	{
		TYPE_INT = 0,
		TYPE_FLOAT,
		TYPE_VECTOR2,
		TYPE_VECTOR3,
		TYPE_VECTOR4,
		TYPE_UBYTE4,
		TYPE_UBYTE4_NORM,
		MAX_VERTEX_ELEMENT_TYPES
	};

	/// Arbitrary vertex declaration element semantics.
	enum VertexElementSemantic
	{
		SEM_POSITION = 0,
		SEM_NORMAL,
		SEM_BINORMAL,
		SEM_TANGENT,
		SEM_TEXCOORD,
		SEM_COLOR,
		SEM_BLENDWEIGHTS,
		SEM_BLENDINDICES,
		SEM_OBJECTINDEX,
		MAX_VERTEX_ELEMENT_SEMANTICS
	};


	uEnum(ValueType,
				"UNDEFINED", "INT8", "INT16", "INT32", "UINT8", "UINT16", "UINT32", "FLOAT16", "FLOAT32")
	
	uEnum(PrimitiveTopology,
				"UNDEFINED", "TRIANGLE_LIST", "TRIANGLE_STRIP", "POINT_LIST", "LINE_LIST", 
				"PATCHES_1", "PATCHES_2", "PATCHES_3", "PATCHES_4", "PATCHES5", "PATCHES6", "PATCHES7", "PATCHES8",
				"PATCHES_9", "PATCHES_10", "PATCHES_11", "PATCHES_12", "PATCHES_13", "PATCHES_14", "PATCHES_16", "PATCHES_16",
				"PATCHES_17", "PATCHES_18", "PATCHES_19", "PATCHES_20", "PATCHES_21", "PATCHES_22", "PATCHES_23", "PATCHES_24",
				"PATCHES_25", "PATCHES_26", "PATCHES_27", "PATCHES_28", "PATCHES_29", "PATCHES_30", "PATCHES_31", "PATCHES_32")

	uEnum(StencilOp, "UNDEFINED", "KEEP", "ZERO", "REPLACE", 
		"INCR_SAT", "DECR_SAT", "INVERT", "INCR_WRAP", "DECR_WRAP")

	uEnum(ComparisonFunction, "UNKNOWN", "NEVER", "LESS", "EQUAL", 
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
		"AND_INVERTED",	"OR_REVERSE", "OR_INVERTED")
		

	ENABLE_BITMASK_OPERATORS(ColorMask)
	uFlags(ColorMask, {"RED", COLOR_MASK_RED}, {"GREEN", COLOR_MASK_GREEN}, {"BLUE", COLOR_MASK_BLUE}, {"ALPHA", COLOR_MASK_ALPHA}, { "ALL", COLOR_MASK_ALL })
	

	static const Vector3 DOT_SCALE(1 / 3.0f, 1 / 3.0f, 1 / 3.0f);

}