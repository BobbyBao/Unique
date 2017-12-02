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

    /// Release buffer.
    virtual void Release();

	bool Create(unsigned indexCount, bool largeIndices, const ByteArray& mem);

    /// Set size and vertex elements and dynamic mode. Previous data will be lost.
    bool SetSize(unsigned indexCount, bool largeIndices, bool dynamic = false);
    /// Set all data in the buffer.
    bool SetData(const void* data);
    /// Set a data range in the buffer. Optionally discard data outside the range.
    bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);
    /// Lock the buffer for write-only editing. Return data pointer if successful. Optionally discard data outside the range.
    void* Lock(unsigned start, unsigned count, bool discard = false);
    /// Unlock the buffer and apply changes to the GPU buffer.
    void Unlock();

    /// Return number of indices.
    unsigned GetIndexCount() const { return elementCount_; }

    /// Return index size in bytes.
    unsigned GetIndexSize() const { return elementSize_; }

    /// Return used vertex range from index range.
    bool GetUsedVertexRange(unsigned start, unsigned count, unsigned& minVertex, unsigned& vertexCount);
	
private:
    /// Create buffer.
    bool Create();

};

}
