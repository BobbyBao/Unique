#include "Precompiled.h"

#include "../Graphics.h"
#include "IndexBuffer.h"
#include "../../IO/Log.h"
#include <LLGL/Utility.h>
#include "../../DebugNew.h"

namespace Unique
{

	uObject(IndexBuffer)
	{
		uFactory("Graphics")
		uAttribute("Data", data_)
	}

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

}
