#pragma once

#include "../../Container/ArrayPtr.h"
#include "../../Core/Object.h"
#include "GraphicsBuffer.h"
#include <LLGL/VertexAttribute.h>

namespace Unique
{
	using VectorType = LLGL::VectorType;
	using VertexFormat = LLGL::VertexFormat;
	using VertexAttribute = LLGL::VertexAttribute;

	/// Hardware vertex buffer.
	class UNIQUE_API VertexBuffer : public GraphicsBuffer
	{
		uRTTI(VertexBuffer, GraphicsBuffer)

	public:
		/// Construct. Optionally force headless (no GPU-side buffer) operation.
		VertexBuffer();
		/// Destruct.
		virtual ~VertexBuffer();
		
		/// Set size, vertex elements and dynamic mode. Previous data will be lost.
		bool Create(uint vertexCount, const VertexFormat& vertexFormat, long flag = 0, void* data = 0);
		
		/// Return number of vertices.
		unsigned GetVertexCount() const { return elementCount_; }

		/// Return vertex size in bytes.
		unsigned GetVertexSize() const { return elementSize_; }

		/// Return vertex size in bytes.
		const VertexFormat& GetVertexFormat() const { return vertexFormat_; }

		/// Return vertex element, or null if does not exist.
		const VertexAttribute* GetElement(const std::string& semantic, unsigned char index = 0) const;

		/// Return vertex element with specific type, or null if does not exist.
		const VertexAttribute* GetElement(VectorType type, const std::string& semantic, unsigned char index = 0) const;

		/// Return whether has a specified element semantic.
		bool HasElement(const std::string& semantic, unsigned char index = 0) const { return GetElement(semantic, index) != 0; }

		/// Return whether has an element semantic with specific type.
		bool HasElement(VectorType type, const std::string& semantic, unsigned char index = 0) const { return GetElement(type, semantic, index) != 0; }

		/// Return offset of a element within vertex, or M_MAX_UNSIGNED if does not exist.
		unsigned GetElementOffset(const std::string& semantic, unsigned char index = 0) const { const VertexAttribute* element = GetElement(semantic, index); return element ? element->offset : M_MAX_UNSIGNED; }

		/// Return offset of a element with specific type within vertex, or M_MAX_UNSIGNED if element does not exist.
		unsigned GetElementOffset(VectorType type, const std::string& semantic, unsigned char index = 0) const { const VertexAttribute* element = GetElement(type, semantic, index); return element ? element->offset : M_MAX_UNSIGNED; }

		/// Return element with specified type and semantic from a vertex element list, or null if does not exist.
		static const VertexAttribute* GetElement(const VertexFormat& vertexFormat, VectorType type, const std::string& semantic, unsigned char index = 0);
	private:
		/// Create buffer.
		virtual bool CreateImpl();

		VertexFormat vertexFormat_;
	};

}
