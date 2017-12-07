#pragma once

#include "GraphicsBuffer.h"


namespace Unique
{

	/// Hardware index buffer.
	class UNIQUE_API IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		/// Construct.
		IndexBuffer();
		/// Destruct.
		virtual ~IndexBuffer();
	
		bool Create(unsigned indexCount, bool largeIndices, long flag = 0, void* data = 0);

		/// Return number of indices.
		unsigned GetIndexCount() const { return elementCount_; }

		/// Return index size in bytes.
		unsigned GetIndexSize() const { return elementSize_; }

		/// Return used vertex range from index range.
		bool GetUsedVertexRange(unsigned start, unsigned count, unsigned& minVertex, unsigned& vertexCount);
	
	private:
		/// Create buffer.
		virtual bool CreateImpl();

	};

}
