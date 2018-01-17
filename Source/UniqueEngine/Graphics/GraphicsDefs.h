#pragma once

#include "Container/Container.h"
#include "Serialize/SerializeTraits.h"


namespace Unique
{
	enum class DeviceType : int
	{
		Undefined = 0,  ///< Undefined device
		D3D11,      ///< D3D11 device
		D3D12,      ///< D3D12 device
		OpenGL,     ///< OpenGL device 
		OpenGLES    ///< OpenGLES device
	};

	enum ValueType : int
	{
		VT_UNDEFINED = 0, ///< Undefined type
		VT_INT8,          ///< Signed 8-bit integer
		VT_INT16,         ///< Signed 16-bit integer
		VT_INT32,         ///< Signed 32-bit integer
		VT_UINT8,         ///< Unsigned 8-bit integer
		VT_UINT16,        ///< Unsigned 16-bit integer
		VT_UINT32,        ///< Unsigned 32-bit integer
		VT_FLOAT16,       ///< Half-precision 16-bit floating point
		VT_FLOAT32,       ///< Full-precision 32-bit floating point
		VT_NUM_TYPES      ///< Helper value storing total number of types in the enumeration
	};

	enum BindFlags : int
	{
		BIND_NONE = 0x0L, ///< Undefined binding
		BIND_VERTEX_BUFFER = 0x1L, ///< A buffer can be bound as a vertex buffer
		BIND_INDEX_BUFFER = 0x2L, ///< A buffer can be bound as an index buffer
		BIND_UNIFORM_BUFFER = 0x4L, ///< A buffer can be bound as a uniform buffer
		///  \warning This flag may not be combined with any other bind flag
		BIND_SHADER_RESOURCE = 0x8L, ///< A buffer or a texture can be bound as a shader resource
		///  \warning This flag cannot be used with MAP_WRITE_NO_OVERWRITE flag 
		BIND_STREAM_OUTPUT = 0x10L,///< A buffer can be bound as a target for stream output stage
		BIND_RENDER_TARGET = 0x20L,///< A texture can be bound as a render target
		BIND_DEPTH_STENCIL = 0x40L,///< A texture can be bound as a depth-stencil target
		BIND_UNORDERED_ACCESS = 0x80L,///< A buffer or a texture can be bound as an unordered access view
		BIND_INDIRECT_DRAW_ARGS = 0x100L///< A buffer can be bound as the source buffer for indirect draw commands
	};

	enum Usage : int
	{
		USAGE_STATIC = 0,
		USAGE_DEFAULT,
		USAGE_DYNAMIC,
		USAGE_CPU_ACCESSIBLE
	};
  
	enum CPUAccessFlag : int
	{
		CPU_ACCESS_READ = 0x01, ///< A resource can be mapped for reading
		CPU_ACCESS_WRITE = 0x02 ///< A resource can be mapped for writing
	};

	enum FilterType : int
	{
		FILTER_TYPE_UNKNOWN = 0,           ///< Unknown filter type
		FILTER_TYPE_POINT,                  ///< Point filtering
		FILTER_TYPE_LINEAR,                 ///< Linear filtering
		FILTER_TYPE_ANISOTROPIC,            ///< Anisotropic filtering
		FILTER_TYPE_COMPARISON_POINT,       ///< Comparison-point filtering
		FILTER_TYPE_COMPARISON_LINEAR,      ///< Comparison-linear filtering
		FILTER_TYPE_COMPARISON_ANISOTROPIC, ///< Comparison-anisotropic filtering
		FILTER_TYPE_MINIMUM_POINT,          ///< Minimum-point filtering (DX12 only)
		FILTER_TYPE_MINIMUM_LINEAR,         ///< Minimum-linear filtering (DX12 only)
		FILTER_TYPE_MINIMUM_ANISOTROPIC,    ///< Minimum-anisotropic filtering (DX12 only)
		FILTER_TYPE_MAXIMUM_POINT,          ///< Maximum-point filtering (DX12 only)
		FILTER_TYPE_MAXIMUM_LINEAR,         ///< Maximum-linear filtering (DX12 only)
		FILTER_TYPE_MAXIMUM_ANISOTROPIC,    ///< Maximum-anisotropic filtering (DX12 only)
		FILTER_TYPE_NUM_FILTERS             ///< Helper value that stores the total number of filter types in the enumeration
	};

	enum class TextureAddressMode : int
	{
		UNKNOWN = 0,
		WRAP = 1,
		MIRROR = 2,
		CLAMP = 3,
		BORDER = 4,
		MIRROR_ONCE = 5,
		TEXTURE_ADDRESS_NUM_MODES
	};

	enum ComparisonFunc : int
	{
		COMPARISON_FUNC_UNKNOWN = 0,
		COMPARISON_FUNC_NEVER,
		COMPARISON_FUNC_LESS,
		COMPARISON_FUNC_EQUAL,
		COMPARISON_FUNC_LESS_EQUAL,
		COMPARISON_FUNC_GREATER,
		COMPARISON_FUNC_NOT_EQUAL,
		COMPARISON_FUNC_GREATER_EQUAL,
		COMPARISON_FUNC_ALWAYS,
		COMPARISON_FUNC_NUM_FUNCTIONS
	};

	enum StencilOp : int
	{
		STENCIL_OP_UNDEFINED = 0,
		STENCIL_OP_KEEP = 1,
		STENCIL_OP_ZERO = 2,
		STENCIL_OP_REPLACE = 3,
		STENCIL_OP_INCR_SAT = 4,
		STENCIL_OP_DECR_SAT = 5,
		STENCIL_OP_INVERT = 6,
		STENCIL_OP_INCR_WRAP = 7,
		STENCIL_OP_DECR_WRAP = 8,
		STENCIL_OP_NUM_OPS
	};

	enum FillMode : int
	{
		FILL_MODE_UNDEFINED = 0,
		FILL_MODE_WIREFRAME,
		FILL_MODE_SOLID,
		FILL_MODE_NUM_MODES
	};

	enum CullMode : int
	{
		CULL_MODE_UNDEFINED = 0,
		CULL_MODE_NONE,
		CULL_MODE_FRONT,
		CULL_MODE_BACK,
		CULL_MODE_NUM_MODES
	};
	
	enum BlendFactor : byte
	{
		BLEND_FACTOR_UNDEFINED = 0,
		BLEND_FACTOR_ZERO,
		BLEND_FACTOR_ONE,
		BLEND_FACTOR_SRC_COLOR,
		BLEND_FACTOR_INV_SRC_COLOR,
		BLEND_FACTOR_SRC_ALPHA,
		BLEND_FACTOR_INV_SRC_ALPHA,
		BLEND_FACTOR_DEST_ALPHA,
		BLEND_FACTOR_INV_DEST_ALPHA,
		BLEND_FACTOR_DEST_COLOR,
		BLEND_FACTOR_INV_DEST_COLOR,
		BLEND_FACTOR_SRC_ALPHA_SAT,
		BLEND_FACTOR_BLEND_FACTOR,
		BLEND_FACTOR_INV_BLEND_FACTOR,
		BLEND_FACTOR_SRC1_COLOR,
		BLEND_FACTOR_INV_SRC1_COLOR,
		BLEND_FACTOR_SRC1_ALPHA,
		BLEND_FACTOR_INV_SRC1_ALPHA, 
		BLEND_FACTOR_NUM_FACTORS
	};

	enum BlendOperation : byte
	{
		BLEND_OPERATION_UNDEFINED = 0,
		BLEND_OPERATION_ADD,
		BLEND_OPERATION_SUBTRACT,
		BLEND_OPERATION_REV_SUBTRACT,
		BLEND_OPERATION_MIN,
		BLEND_OPERATION_MAX,
		BLEND_OPERATION_NUM_OPERATIONS
	};

	enum LogicOperation : byte
	{
		LOGIC_OP_CLEAR = 0,
		LOGIC_OP_SET,
		LOGIC_OP_COPY,
		LOGIC_OP_COPY_INVERTED,
		LOGIC_OP_NOOP,
		LOGIC_OP_INVERT,
		LOGIC_OP_AND,
		LOGIC_OP_NAND,
		LOGIC_OP_OR,
		LOGIC_OP_NOR,
		LOGIC_OP_XOR,
		LOGIC_OP_EQUIV,
		LOGIC_OP_AND_REVERSE,
		LOGIC_OP_AND_INVERTED,
		LOGIC_OP_OR_REVERSE,
		LOGIC_OP_OR_INVERTED,
		LOGIC_OP_NUM_OPERATIONS
	};

	enum ColorMask : int
	{
		COLOR_MASK_RED	    = 1,
		COLOR_MASK_GREEN	= 2,
		COLOR_MASK_BLUE	    = 4,
		COLOR_MASK_ALPHA	= 8,
		COLOR_MASK_ALL	= ( ( ( COLOR_MASK_RED | COLOR_MASK_GREEN )  | COLOR_MASK_BLUE )  | COLOR_MASK_ALPHA ) 
	};

	enum class PrimitiveTopology : byte
	{
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
	enum VertexElementType : byte
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
	enum VertexElementSemantic : byte
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
		"AND_INVERTED",	"OR_REVERSE", "OR_INVERTED")
		

	ENABLE_BITMASK_OPERATORS(ColorMask)
	uFlags(ColorMask, {"RED", COLOR_MASK_RED}, {"GREEN", COLOR_MASK_GREEN}, {"BLUE", COLOR_MASK_BLUE}, {"ALPHA", COLOR_MASK_ALPHA}, { "ALL", COLOR_MASK_ALL })

	/// Vertex element description for arbitrary vertex declarations.
	
	struct UNIQUE_API VertexElement
	{
		/// Default-construct.
		VertexElement() :
			type_(TYPE_VECTOR3),
			semantic_(SEM_POSITION),
			index_(0),
			perInstance_(false),
			offset_(0)
		{
		}

		/// Construct with type, semantic, index and whether is per-instance data.
		VertexElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0, bool perInstance = false) :
			type_(type),
			semantic_(semantic),
			index_(index),
			perInstance_(perInstance),
			offset_(0)
		{
		}

		/// Test for equality with another vertex element. Offset is intentionally not compared, as it's relevant only when an element exists within a vertex buffer.
		bool operator ==(const VertexElement& rhs) const { return type_ == rhs.type_ && semantic_ == rhs.semantic_ && index_ == rhs.index_ && perInstance_ == rhs.perInstance_; }

		/// Test for inequality with another vertex element.
		bool operator !=(const VertexElement& rhs) const { return !(*this == rhs); }

		/// Data type of element.
		VertexElementType type_;
		/// Semantic of element.
		VertexElementSemantic semantic_;
		/// Semantic index of element, for example multi-texcoords.
		unsigned char index_;
		/// Per-instance flag.
		bool perInstance_;
		/// Offset of element from vertex start. Filled by VertexBuffer once the vertex declaration is built.
		unsigned offset_;

	};

	static const Vector3 DOT_SCALE(1 / 3.0f, 1 / 3.0f, 1 / 3.0f);

}