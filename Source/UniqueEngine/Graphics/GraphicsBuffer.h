#pragma once
#include "Core/Object.h"
#include <Buffer.h>
#include "GPUObject.h"

namespace Unique
{
	class UNIQUE_API GraphicsBuffer : public GPUObject<Object, IBuffer>
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer(uint flags = BIND_NONE);
		~GraphicsBuffer();

		/// Set size, vertex elements and dynamic mode. Previous data will be lost.
		bool Create(uint elementCount, uint elementSize, Usage usage = Usage::USAGE_DEFAULT, void* data = 0);

		bool SetData(const void* data);
		/// Set a data range in the buffer. Optionally discard data outside the range.
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);

		inline bool IsDynamic() const { return desc_.Usage == USAGE::USAGE_DYNAMIC; }
		inline uint GetSizeInBytes() const { return desc_.uiSizeInBytes; }
		inline uint GetCount() const { return desc_.uiSizeInBytes / desc_.ElementByteStride; }
		inline uint GetStride() const { return desc_.ElementByteStride; }

		virtual void UpdateBuffer();
	protected:
		/// Create buffer.
		virtual bool CreateImpl();

		BufferDesc desc_;
		ByteArray data_;
		ByteArray data1_;
	};

	/// Hardware vertex buffer.
	class UNIQUE_API VertexBuffer : public GraphicsBuffer
	{
		uRTTI(VertexBuffer, GraphicsBuffer)

	public:
		/// Construct. Optionally force headless (no GPU-side buffer) operation.
		VertexBuffer();
	};

	/// Hardware index buffer.
	class UNIQUE_API IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		/// Construct.
		IndexBuffer();

		/// Return used vertex range from index range.
		bool GetUsedVertexRange(unsigned start, unsigned count, unsigned& minVertex, unsigned& vertexCount);

	};
}
