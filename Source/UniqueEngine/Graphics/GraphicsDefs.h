#pragma once

#include "Container/Container.h"
#include "Serialize/SerializeTraits.h"

#include <GraphicsTypes.h>
#include <InputLayout.h>
#include <RasterizerState.h>

using namespace Diligent;

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
	
	enum ClearFlags : int
	{
		CLEAR_NONE = 0,
		CLEAR_COLOR = 0x01,
		CLEAR_DEPTH = 0x02,    ///< Clear depth part of the buffer
		CLEAR_STENCIL = 0x04   ///< Clear stencil part of the buffer
	};

	//using ClearFlags = Diligent::;
	using ValueType = Diligent::VALUE_TYPE;
	using BindFlags = Diligent::BIND_FLAGS;
	using Usage = Diligent::USAGE;
	using CPUAccessFlag = Diligent::CPU_ACCESS_FLAGS;
	using FillMode = Diligent::FILL_MODE;
	using CullMode = Diligent::CULL_MODE;
	using PrimitiveTopology = Diligent::PRIMITIVE_TOPOLOGY;


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
	
	/// Cube map faces.
	enum CubeMapFace
	{
		FACE_POSITIVE_X = 0,
		FACE_NEGATIVE_X,
		FACE_POSITIVE_Y,
		FACE_NEGATIVE_Y,
		FACE_POSITIVE_Z,
		FACE_NEGATIVE_Z,
		MAX_CUBEMAP_FACES
	};

	/// Cubemap single image layout modes.
	enum CubeMapLayout
	{
		CML_HORIZONTAL = 0,
		CML_HORIZONTALNVIDIA,
		CML_HORIZONTALCROSS,
		CML_VERTICALCROSS,
		CML_BLENDER
	};

	
	/// Texture units.
	enum TextureUnit
	{
		TU_DIFFUSE = 0,
		TU_ALBEDOBUFFER = 0,
		TU_NORMAL = 1,
		TU_NORMALBUFFER = 1,
		TU_SPECULAR = 2,
		TU_EMISSIVE = 3,
		TU_ENVIRONMENT = 4,
		TU_VOLUMEMAP = 5,
		TU_CUSTOM1 = 6,
		TU_CUSTOM2 = 7,
		TU_LIGHTRAMP = 8,
		TU_LIGHTSHAPE = 9,
		TU_SHADOWMAP = 10,
		TU_FACESELECT = 11,
		TU_INDIRECTION = 12,
		TU_DEPTHBUFFER = 13,
		TU_LIGHTBUFFER = 14,
		TU_ZONE = 15,
		MAX_MATERIAL_TEXTURE_UNITS = 8,
		MAX_TEXTURE_UNITS = 16
	};

	/// Shader parameter groups for determining need to update. On APIs that support constant buffers, these correspond to different constant buffers.
	enum ShaderParameterGroup
	{
		SP_FRAME = 0,
		SP_CAMERA,
		SP_ZONE,
		SP_LIGHT,
		SP_MATERIAL,
		SP_OBJECT,
		SP_CUSTOM,
		MAX_SHADER_PARAMETER_GROUPS
	};

	/// Billboard camera facing modes.
	enum FaceCameraMode
	{
		FC_NONE = 0,
		FC_ROTATE_XYZ,
		FC_ROTATE_Y,
		FC_LOOKAT_XYZ,
		FC_LOOKAT_Y,
		FC_LOOKAT_MIXED,
		FC_DIRECTION,
	};

	uFlags(ClearFlags, {"COLOR", CLEAR_COLOR_FLAG}, {"DEPTH", CLEAR_DEPTH_FLAG}, {"STENCIL", CLEAR_STENCIL_FLAG})

	uEnum(ValueType, "UNDEFINED", "INT8", "INT16", "INT32", "UINT8", "UINT16", "UINT32", "FLOAT16", "FLOAT32")
	
	uEnum(PrimitiveTopology,
				"UNDEFINED", "TRIANGLE_LIST", "TRIANGLE_STRIP", "POINT_LIST", "LINE_LIST", 
				"PATCHES_1", "PATCHES_2", "PATCHES_3", "PATCHES_4", "PATCHES5", "PATCHES6", "PATCHES7", "PATCHES8",
				"PATCHES_9", "PATCHES_10", "PATCHES_11", "PATCHES_12", "PATCHES_13", "PATCHES_14", "PATCHES_16", "PATCHES_16",
				"PATCHES_17", "PATCHES_18", "PATCHES_19", "PATCHES_20", "PATCHES_21", "PATCHES_22", "PATCHES_23", "PATCHES_24",
				"PATCHES_25", "PATCHES_26", "PATCHES_27", "PATCHES_28", "PATCHES_29", "PATCHES_30", "PATCHES_31", "PATCHES_32")

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
	static const int MAX_RENDERTARGETS = 4;
	static const int MAX_VERTEX_STREAMS = 4;
}