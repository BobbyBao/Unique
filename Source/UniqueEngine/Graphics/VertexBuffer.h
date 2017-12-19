#pragma once

#include "GraphicsBuffer.h"

namespace Unique
{

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

	class UNIQUE_API VertexBuffer : public GraphicsBuffer
	{
		uRTTI(VertexBuffer, GraphicsBuffer)

	public:
		VertexBuffer() : GraphicsBuffer(BIND_VERTEX_BUFFER) {}
		VertexBuffer(uint elementMask, ByteArray&& data, Usage usage = Usage::USAGE_STATIC);
		VertexBuffer(const PODVector<VertexElement>& elements, ByteArray&& data, Usage usage = Usage::USAGE_STATIC);

		/// Return vertex elements.
		const PODVector<VertexElement>& GetElements() const { return elements_; }

		/// Return vertex element, or null if does not exist.
		const VertexElement* GetElement(VertexElementSemantic semantic, unsigned char index = 0) const;

		/// Return vertex element with specific type, or null if does not exist.
		const VertexElement* GetElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const;

		/// Return whether has a specified element semantic.
		bool HasElement(VertexElementSemantic semantic, unsigned char index = 0) const { return GetElement(semantic, index) != nullptr; }

		/// Return whether has an element semantic with specific type.
		bool HasElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const { return GetElement(type, semantic, index) != nullptr; }

		/// Return offset of a element within vertex, or M_MAX_UNSIGNED if does not exist.
		unsigned GetElementOffset(VertexElementSemantic semantic, unsigned char index = 0) const { const VertexElement* element = GetElement(semantic, index); return element ? element->offset_ : M_MAX_UNSIGNED; }

		/// Return offset of a element with specific type within vertex, or M_MAX_UNSIGNED if element does not exist.
		unsigned GetElementOffset(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const { const VertexElement* element = GetElement(type, semantic, index); return element ? element->offset_ : M_MAX_UNSIGNED; }
		
		/// Return buffer hash for building vertex declarations. Used internally.
		unsigned long long GetBufferHash(unsigned streamIndex) { return elementHash_ << (streamIndex * 16); }

		/// Return element with specified type and semantic from a vertex element list, or null if does not exist.
		static const VertexElement* GetElement(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0);

		/// Return whether element list has a specified element type and semantic.
		static bool HasElement(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0);

		/// Return element offset for specified type and semantic from a vertex element list, or M_MAX_UNSIGNED if does not exist.
		static unsigned GetElementOffset(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0);

		/// Return a vertex element list from a legacy element bitmask.
		static PODVector<VertexElement> GetElements(unsigned elementMask);

		/// Return vertex size from an element list.
		static unsigned GetVertexSize(const PODVector<VertexElement>& elements);

		/// Return vertex size for a legacy vertex element bitmask.
		static unsigned GetVertexSize(unsigned elementMask);

		/// Update offsets of vertex elements.
		static void UpdateOffsets(PODVector<VertexElement>& elements);

	private:
		/// Update offsets of vertex elements.
		void UpdateOffsets();
		/// Vertex elements.
		PODVector<VertexElement> elements_;
		/// Vertex element hash.
		unsigned long long elementHash_;
	};

	/// Sizes of vertex element types.
	extern UNIQUE_API const unsigned ELEMENT_TYPESIZES[];

	/// Vertex element definitions for the legacy elements.
	extern UNIQUE_API const VertexElement LEGACY_VERTEXELEMENTS[];


	// Legacy vertex element bitmasks.
	static const unsigned MASK_NONE = 0x0;
	static const unsigned MASK_POSITION = 0x1;
	static const unsigned MASK_NORMAL = 0x2;
	static const unsigned MASK_COLOR = 0x4;
	static const unsigned MASK_TEXCOORD1 = 0x8;
	static const unsigned MASK_TEXCOORD2 = 0x10;
	static const unsigned MASK_CUBETEXCOORD1 = 0x20;
	static const unsigned MASK_CUBETEXCOORD2 = 0x40;
	static const unsigned MASK_TANGENT = 0x80;
	static const unsigned MASK_BLENDWEIGHTS = 0x100;
	static const unsigned MASK_BLENDINDICES = 0x200;
	static const unsigned MASK_INSTANCEMATRIX1 = 0x400;
	static const unsigned MASK_INSTANCEMATRIX2 = 0x800;
	static const unsigned MASK_INSTANCEMATRIX3 = 0x1000;
	static const unsigned MASK_OBJECTINDEX = 0x2000;
}

