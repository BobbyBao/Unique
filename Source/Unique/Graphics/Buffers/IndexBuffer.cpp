#include "Precompiled.h"

#include "../Graphics.h"
#include "IndexBuffer.h"
#include "../../IO/Log.h"
#include <LLGL/Utility.h>
#include "../../DebugNew.h"

namespace Unique
{

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
    Release();
}

bool IndexBuffer::Create(unsigned indexCount, bool largeIndices, long flag, void* data)
{
	elementSize_ = (unsigned)(largeIndices ? sizeof(unsigned) : sizeof(unsigned short));
	elementCount_ = indexCount;
	flags_ = flag;

	data_.resize(elementCount_ * elementSize_);
	if (data)
	{
		std::memcpy(data_.data(), data, elementCount_*elementSize_);
	}

	return GraphicsBuffer::Create();
	
}

bool IndexBuffer::CreateImpl()
{
	ReleaseImpl();

	if (!elementCount_ || !elementSize_)
		return true;


	handle_ = renderer->CreateBuffer(IndexBufferDesc((uint)data_.size(), 
		IndexFormat(elementSize_ == 4 ?LLGL::DataType::UInt32 : LLGL::DataType::UInt16), flags_), data_.data());
	return handle_ != nullptr;

}

bool IndexBuffer::GetUsedVertexRange(unsigned start, unsigned count, unsigned& minVertex, unsigned& vertexCount)
{
    if (data_.empty())
    {
        UNIQUE_LOGERROR("Used vertex range can only be queried from an index buffer with shadow data");
        return false;
    }

    if (start + count > elementCount_)
    {
        UNIQUE_LOGERROR("Illegal index range for querying used vertices");
        return false;
    }

    minVertex = M_MAX_UNSIGNED;
    unsigned maxVertex = 0;

    if (elementSize_ == sizeof(unsigned))
    {
        unsigned* indices = (unsigned*)data_.data() + start;

        for (unsigned i = 0; i < count; ++i)
        {
            if (indices[i] < minVertex)
                minVertex = indices[i];
            if (indices[i] > maxVertex)
                maxVertex = indices[i];
        }
    }
    else
    {
        unsigned short* indices = (unsigned short*)data_.data() + start;

        for (unsigned i = 0; i < count; ++i)
        {
            if (indices[i] < minVertex)
                minVertex = indices[i];
            if (indices[i] > maxVertex)
                maxVertex = indices[i];
        }
    }

    vertexCount = maxVertex - minVertex + 1;
    return true;
}

bool IndexBuffer::SetData(const void* data)
{
	if (!data)
	{
		UNIQUE_LOGERROR("Null pointer for index buffer data");
		return false;
	}

	if (!elementSize_)
	{
		UNIQUE_LOGERROR("Index size not defined, can not set index buffer data");
		return false;
	}

	/*

	const bgfx::Memory* mem = nullptr;
	if (shadowData_)
	{
		if (data != shadowData_.Get())
			memcpy(shadowData_.Get(), data, elementCount_ * elementSize_);

		mem = bgfx::makeRef(shadowData_.Get(), elementCount_ * elementSize_);
	}
	else
	{
		mem = bgfx::copy(data, elementCount_ * elementSize_);
	}

	if (IsDynamic())
	{
		bgfx::updateDynamicIndexBuffer({ handle_ }, 0, mem);
	}
	else
	{
		if (valid())
		{
			Release();
		}

		handle_ = bgfx::createIndexBuffer(mem).idx;
	}*/

	return true;
}

bool IndexBuffer::SetDataRange(const void* data, unsigned start, unsigned count, bool discard)
{
	if (start == 0 && count == elementCount_)
		return SetData(data);

	if (!data)
	{
		UNIQUE_LOGERROR("Null pointer for index buffer data");
		return false;
	}

	if (!elementSize_)
	{
		UNIQUE_LOGERROR("Index size not defined, can not set index buffer data");
		return false;
	}

	if (start + count > elementCount_)
	{
		UNIQUE_LOGERROR("Illegal range for setting new index buffer data");
		return false;
	}

	if (!count)
		return true;
	/*
	const bgfx::Memory* mem = nullptr;
	if (shadowData_)
	{
		if (shadowData_.Get() + start * elementSize_ != data)
			memcpy(shadowData_.Get() + start * elementSize_, data, count * elementSize_);

		mem = bgfx::makeRef(shadowData_.Get(), (count + start) * elementSize_);
	}
	else
	{
		//to do check
		mem = bgfx::copy(((unsigned char*)data + start * elementSize_), count * elementSize_);
	}

	switch (usage_)
	{
	case Unique::BU_Static:

		if (handle_ != bgfx::kInvalidHandle)
		{
			Release();
		}

		handle_ = bgfx::createIndexBuffer(mem).idx;
		break;
	case Unique::BU_Dynamic:
		bgfx::updateDynamicIndexBuffer({ handle_ }, start, mem);
		break;
	default:
		break;
	}*/

	return true;
}

void* IndexBuffer::Lock(unsigned start, unsigned count, bool discard)
{/*
	if (lockState_)
	{
		UNIQUE_LOGERROR("Index buffer already locked");
		return 0;
	}

	if (!elementSize_)
	{
		UNIQUE_LOGERROR("Index size not defined, can not lock index buffer");
		return 0;
	}

	if (start + count > elementCount_)
	{
		UNIQUE_LOGERROR("Illegal range for locking index buffer");
		return 0;
	}

	if (!count)
		return 0;

	lockStart_ = start;
	lockCount_ = count;

	if (shadowData_)
	{
		lockState_ = true;
		return shadowData_.Get() + start * elementSize_;
	}
	else*/
		return 0;
}

void IndexBuffer::Unlock()
{/*
	if (lockState_)
	{
		SetDataRange(shadowData_.Get() + lockStart_ * elementSize_, lockStart_, lockCount_);
		lockState_ = false;
	}*/
}


}
