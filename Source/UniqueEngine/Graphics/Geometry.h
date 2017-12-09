//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "Container/ArrayPtr.h"
#include "Core/Object.h"
#include "../Graphics/GraphicsDefs.h"
#include "../Graphics/GPUObject.h"

namespace Unique
{

class IndexBuffer;
class Ray;
class Graphics;
class VertexBuffer;
class PipelineState;

/// Defines one or more vertex buffers, an index buffer and a draw range.
class UNIQUE_API Geometry : public Object
{
    uRTTI(Geometry, Object)

public:
    /// Construct with one empty vertex buffer.
    Geometry();
    /// Destruct.
    virtual ~Geometry();

    /// Set number of vertex buffers.
    bool SetNumVertexBuffers(unsigned num);
    /// Set a vertex buffer by index.
    bool SetVertexBuffer(unsigned index, VertexBuffer* buffer);
    /// Set the index buffer.
    void SetIndexBuffer(IndexBuffer* buffer);
    /// Set the draw range.
    bool SetDrawRange(PrimitiveTopology type, unsigned indexStart, unsigned indexCount);
    /// Set the draw range.
    bool SetDrawRange(PrimitiveTopology type, unsigned indexStart, unsigned indexCount, unsigned vertexStart, unsigned vertexCount);
    /// Set the LOD distance.
    void SetLodDistance(float distance);

	void Draw(PipelineState* pipeline, IResourceMapping* resourceMapping);
    
	/// Return all vertex buffers.
    const Vector<SPtr<VertexBuffer> >& GetVertexBuffers() const { return vertexBuffers_; }

    /// Return number of vertex buffers.
    unsigned GetNumVertexBuffers() const { return (unsigned)vertexBuffers_.size(); }

    /// Return vertex buffer by index.
    VertexBuffer* GetVertexBuffer(unsigned index) const;

    /// Return the index buffer.
    IndexBuffer* GetIndexBuffer() const { return indexBuffer_; }

    /// Return primitive type.
    PrimitiveTopology GetPrimitiveType() const { return drawAttribs_.Topology; }

    /// Return start index.
    unsigned GetIndexStart() const { return indexStart_; }

    /// Return number of indices.
    unsigned GetIndexCount() const { return indexCount_; }

    /// Return first used vertex.
    unsigned GetVertexStart() const { return vertexStart_; }

    /// Return number of used vertices.
    unsigned GetVertexCount() const { return vertexCount_; }

    /// Return LOD distance.
    float GetLodDistance() const { return lodDistance_; }

    /// Return buffers' combined hash value for state sorting.
    unsigned short GetBufferHash() const;

    /// Return ray hit distance or infinity if no hit. Requires raw data to be set. Optionally return hit normal and hit uv coordinates at intersect point.
    float GetHitDistance(const Ray& ray, Vector3* outNormal = 0, Vector2* outUV = 0) const;
    /// Return whether or not the ray is inside geometry.
    bool IsInside(const Ray& ray) const;

    /// Return whether has empty draw range.
    bool IsEmpty() const { return indexCount_ == 0 && vertexCount_ == 0; }

private:
    /// Vertex buffers.
    Vector<SPtr<VertexBuffer> > vertexBuffers_;
    /// Index buffer.
	SPtr<IndexBuffer> indexBuffer_;

	DrawAttribs drawAttribs_;

    /// Start index.
    unsigned indexStart_;
    /// Number of indices.
    unsigned indexCount_;
    /// First used vertex.
    unsigned vertexStart_;
    /// Number of used vertices.
    unsigned vertexCount_;
    /// LOD distance.
    float lodDistance_;
};

}
