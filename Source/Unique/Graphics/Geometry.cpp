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

#include "Precompiled.h"

#include "../Graphics/Geometry.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Buffers/IndexBuffer.h"
#include "../Graphics/Buffers/VertexBuffer.h"
#include "../Graphics/Shader/Shader.h"
#include "IO/Log.h"
//#include "../Math/Ray.h"


#define MAX_VERTEX_STREAMS 8

namespace Unique
{
	uObject(Geometry)
	{
		uFactory("Graphics")
			uAttribute("PrimitiveType", primitiveType_)
			uAttribute("VertexBuffers", vertexBuffers_)
			uAttribute("IndexStart", indexStart_)
			uAttribute("IndexCount", indexCount_)
			uAttribute("VertexStart", vertexStart_)
			uAttribute("VertexCount", vertexCount_)

	}

Geometry::Geometry() :
    primitiveType_(PrimitiveType::TriangleList),
    indexStart_(0),
    indexCount_(0),
    vertexStart_(0),
    vertexCount_(0),
    lodDistance_(0.0f)
{
    SetNumVertexBuffers(1);
}

Geometry::~Geometry()
{
}

bool Geometry::SetNumVertexBuffers(unsigned num)
{
    if (num >= MAX_VERTEX_STREAMS)
    {
        UNIQUE_LOGERROR("Too many vertex streams");
        return false;
    }

    unsigned oldSize = (unsigned)vertexBuffers_.size();
    vertexBuffers_.resize(num);
    return true;
}

bool Geometry::SetVertexBuffer(unsigned index, VertexBuffer* buffer)
{
    if (index >= (unsigned)vertexBuffers_.size())
    {
        UNIQUE_LOGERROR("Stream index out of bounds");
        return false;
    }

	if (vertexBuffers_[index] != buffer)
	{
		vertexBuffers_[index] = buffer;

		bool full = true;
		for (auto& vb : vertexBuffers_)
		{
			if (vb == nullptr)
			{
				full = false;
			}
		}

		if (full)
		{
			Create();
		}

	}

    return true;
}

void Geometry::SetIndexBuffer(IndexBuffer* buffer)
{
    indexBuffer_ = buffer;
}

bool Geometry::SetDrawRange(PrimitiveType type, unsigned indexStart, unsigned indexCount, bool getUsedVertexRange)
{
    if (!indexBuffer_)
    {
        UNIQUE_LOGERROR("Null index buffer and no raw index data, can not define indexed draw range");
        return false;
    }
    if (indexBuffer_ && indexStart + indexCount > indexBuffer_->GetIndexCount())
    {
        UNIQUE_LOGERROR("Illegal draw range " + String(indexStart) + " to " + String(indexStart + indexCount - 1) + ", index buffer has " +
                 String(indexBuffer_->GetIndexCount()) + " indices");
        return false;
    }
	
    primitiveType_ = type;
    indexStart_ = indexStart;
    indexCount_ = indexCount;

    // Get min.vertex index and num of vertices from index buffer. If it fails, use full range as fallback
    if (indexCount)
    {
        vertexStart_ = 0;
        vertexCount_ = vertexBuffers_[0] ? vertexBuffers_[0]->GetVertexCount() : 0;

        if (getUsedVertexRange && indexBuffer_)
            indexBuffer_->GetUsedVertexRange(indexStart_, indexCount_, vertexStart_, vertexCount_);
    }
    else
    {
        vertexStart_ = 0;
        vertexCount_ = 0;
    }

    return true;
}

bool Geometry::SetDrawRange(PrimitiveType type, unsigned indexStart, unsigned indexCount, unsigned minVertex, unsigned vertexCount,
    bool checkIllegal)
{
    if (indexBuffer_)
    {
        // We can allow setting an illegal draw range now if the caller guarantees to resize / fill the buffer later
        if (checkIllegal && indexStart + indexCount > indexBuffer_->GetIndexCount())
        {
            UNIQUE_LOGERROR("Illegal draw range " + String(indexStart) + " to " + String(indexStart + indexCount - 1) +
                     ", index buffer has " + String(indexBuffer_->GetIndexCount()) + " indices");
            return false;
        }
    }
    else
    {
        indexStart = 0;
        indexCount = 0;
    }

    primitiveType_ = type;
    indexStart_ = indexStart;
    indexCount_ = indexCount;
    vertexStart_ = minVertex;
    vertexCount_ = vertexCount;

    return true;
}

void Geometry::SetLodDistance(float distance)
{
    if (distance < 0.0f)
        distance = 0.0f;

    lodDistance_ = distance;
}


bool Geometry::CreateImpl()
{
	ReleaseImpl();

	LLGL::Buffer* buffers[MAX_VERTEX_STREAMS];
	for (int i = 0; i < vertexBuffers_.size(); i++)
	{
		buffers[i] = vertexBuffers_[i]->GetHandle();
	}

	handle_ = renderer->CreateBufferArray(vertexBuffers_.size(), buffers);
	return true;
}

void Geometry::Draw(Graphics* graphics, ShaderInstance* shader)
{
	GraphicsPipeline* pipeline = shader->GetPipeline(vertexBuffers_[0]->GetVertexFormat());
	if (indexBuffer_ && indexCount_ > 0)
	{
		graphics->SetVertexBuffers(handle_);
		graphics->SetIndexBuffer(indexBuffer_);
		graphics->SetGraphicsPipeline(pipeline);
		graphics->DrawIndexed(indexCount_, indexStart_, vertexStart_);
	}
	else if (vertexCount_ > 0)
	{
		graphics->SetVertexBuffers(handle_);
		graphics->SetGraphicsPipeline(pipeline);
		graphics->Draw(vertexCount_, vertexStart_);
	}
}

VertexBuffer* Geometry::GetVertexBuffer(unsigned index) const
{
    return index < vertexBuffers_.size() ? vertexBuffers_[index] : (VertexBuffer*)0;
}

unsigned short Geometry::GetBufferHash() const
{
    unsigned short hash = 0;

    for (unsigned i = 0; i < vertexBuffers_.size(); ++i)
    {
        VertexBuffer* vBuf = vertexBuffers_[i];
        hash += *((unsigned short*)&vBuf);
    }

    IndexBuffer* iBuf = indexBuffer_;
    hash += *((unsigned short*)&iBuf);

    return hash;
}
/*
float Geometry::GetHitDistance(const Ray& ray, Vector3* outNormal, Vector2* outUV) const
{
    const unsigned char* vertexData;
    const unsigned char* indexData;
    unsigned vertexSize;
    unsigned indexSize;
    const PODVector<VertexElement>* elements;

    GetRawData(vertexData, vertexSize, indexData, indexSize, elements);
    
    if (!vertexData || !elements || VertexBuffer::GetElementOffset(*elements, TYPE_VECTOR3, SEM_POSITION) != 0)
        return M_INFINITY;

    unsigned uvOffset = VertexBuffer::GetElementOffset(*elements, TYPE_VECTOR2, SEM_TEXCOORD);
    
    if (outUV && uvOffset == M_MAX_UNSIGNED)
    {
        // requested UV output, but no texture data in vertex buffer
        UNIQUE_LOGWARNING("Illegal GetHitDistance call: UV return requested on vertex buffer without UV coords");
        *outUV = Vector2::Z;
        outUV = 0;
    }

    return indexData ? ray.HitDistance(vertexData, vertexSize, indexData, indexSize, indexStart_, indexCount_, outNormal, outUV,
        uvOffset) : ray.HitDistance(vertexData, vertexSize, vertexStart_, vertexCount_, outNormal, outUV, uvOffset);
}
*/


}
