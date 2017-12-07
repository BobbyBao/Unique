#include "Precompiled.h"
#include "GraphicsBuffer.h"

namespace Unique
{
	uObject(GraphicsBuffer)
	{
		uAttribute("ElementSize", elementSize_)
		uAttribute("ElementCount", elementCount_)
	}
	
	GraphicsBuffer::GraphicsBuffer()
	{
	}

	GraphicsBuffer::~GraphicsBuffer()
	{
	}

	bool GraphicsBuffer::SetData(const void* data)
	{
		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (!elementSize_)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if (IsDynamic())
		{
			ByteArray& currentData = GraphicsContext::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data(), data, currentData.size());
		}
		else
		{
			std::memcpy(data_.data(), data, data_.size());
			/*
			GraphicsContext::AddCommand([=]()
			{
				void* buffer = renderer->MapBuffer(*this, LLGL::BufferCPUAccess::WriteOnly);
				memcpy(buffer, data_.data(), data_.size());
				renderer->UnmapBuffer(*this);
			});*/
		}


		return true;
	}

	bool GraphicsBuffer::SetDataRange(const void* data, unsigned start, unsigned count, bool discard)
	{
		if (start == 0 && count == elementCount_)
			return SetData(data);

		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (!elementSize_)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if (start + count > elementCount_)
		{
			UNIQUE_LOGERROR("Illegal range for setting new vertex buffer data");
			return false;
		}

		if (!count)
			return true;

		if (IsDynamic())
		{
			ByteArray& currentData = GraphicsContext::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data() + start * elementSize_, data, count * elementSize_);
		}
		else
		{
			std::memcpy(data_.data() + start * elementSize_, data, count * elementSize_);
			/*
			GraphicsContext::AddCommand([=]()
			{
				void* buffer = renderer->MapBuffer(*this, LLGL::BufferCPUAccess::WriteOnly);
				memcpy(buffer, data_.data() + start * elementSize_, count * elementSize_);
				renderer->UnmapBuffer(*this);
			});*/
		}

		return true;
	}

	void GraphicsBuffer::UpdateBuffer()
	{

	}
}
