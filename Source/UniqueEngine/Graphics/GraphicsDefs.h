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
	using ComparisonFunction = COMPARISON_FUNCTION;
	using ComponentType = COMPONENT_TYPE;

	uEnumTraits(ValueType,
				"UNDEFINED", "INT8", "INT16", "INT32", "UINT8", "UINT16", "UINT32", "FLOAT16", "FLOAT32")
	
	uEnumTraits(PrimitiveTopology,
				"UNDEFINED", "TRIANGLE_LIST", "TRIANGLE_STRIP", "POINT_LIST", "LINE_LIST", 
				"PATCHES_1", "PATCHES_2", "PATCHES_3", "PATCHES_4", "PATCHES5", "PATCHES6", "PATCHES7", "PATCHES8",
				"PATCHES_9", "PATCHES_10", "PATCHES_11", "PATCHES_12", "PATCHES_13", "PATCHES_14", "PATCHES_16", "PATCHES_16",
				"PATCHES_17", "PATCHES_18", "PATCHES_19", "PATCHES_20", "PATCHES_21", "PATCHES_22", "PATCHES_23", "PATCHES_24",
				"PATCHES_25", "PATCHES_26", "PATCHES_27", "PATCHES_28", "PATCHES_29", "PATCHES_30", "PATCHES_31", "PATCHES_32")

	uEnumTraits(ComparisonFunction, "UNKNOWN", "NEVER", "LESS", "EQUAL", 
		"LESS_EQUAL", "GREATER", "NOT_EQUAL", "GREATER_EQUAL", "ALWAYS")

	//uClassTraits(DepthState, "TestEnabled", self.testEnabled, "WriteEnabled", self.writeEnabled, "CompareOp", self.compareOp)

}