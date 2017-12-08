#include "Precompiled.h"
#include "GraphicsBuffer.h"
#include "Graphics/Graphics.h"

namespace Unique
{
	uObject(GraphicsBuffer)
	{
	}
	
	GraphicsBuffer::GraphicsBuffer(uint flags)
	{
		desc_.BindFlags = flags;
	}

	GraphicsBuffer::~GraphicsBuffer()
	{
	}

	bool GraphicsBuffer::Create(uint elementCount, uint elementSize, Usage usage, void* data)
	{
		desc_.ElementByteStride = elementSize;
		desc_.uiSizeInBytes = elementCount*elementSize;
		desc_.Usage = usage;

		data_.resize(desc_.uiSizeInBytes);
		if (data)
		{
			std::memcpy(data_.data(), data, desc_.uiSizeInBytes);
		}

		return GPUObject<Object, IBuffer>::Create();
	}

	bool GraphicsBuffer::CreateImpl()
	{
		if (!GetSizeInBytes())
			return false;

		BufferData BuffData;
		BuffData.pData = data_.data();
		BuffData.DataSize = desc_.uiSizeInBytes;
		renderDevice->CreateBuffer(desc_, BuffData, &handle_);
		return handle_ != nullptr;
	}

	bool GraphicsBuffer::SetData(const void* data)
	{
		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (GetSizeInBytes() == 0)
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
		if (start == 0 && count == GetCount())
			return SetData(data);

		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (GetStride() == 0)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if (start + count > GetCount())
		{
			UNIQUE_LOGERROR("Illegal range for setting new vertex buffer data");
			return false;
		}

		if (!count)
			return true;

		if (IsDynamic())
		{
			ByteArray& currentData = GraphicsContext::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data() + start * GetStride(), data, count * GetStride());
		}
		else
		{
			std::memcpy(data_.data() + start * GetStride(), data, count * GetStride());
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

	uObject(VertexBuffer)
	{
		uFactory("Graphics")
			//	uAttribute("VertexFormat", vertexFormat_)
			uAttribute("Data", data_)
	}

	VertexBuffer::VertexBuffer() : GraphicsBuffer(BIND_VERTEX_BUFFER)
	{
	}

	uObject(IndexBuffer)
	{
		uFactory("Graphics")
			uAttribute("Data", data_)
	}

	IndexBuffer::IndexBuffer() : GraphicsBuffer(BIND_INDEX_BUFFER)
	{
	}

	bool IndexBuffer::GetUsedVertexRange(unsigned start, unsigned count, unsigned& minVertex, unsigned& vertexCount)
	{
		if (data_.empty())
		{
			UNIQUE_LOGERROR("Used vertex range can only be queried from an index buffer with shadow data");
			return false;
		}

		if (start + count > GetCount())
		{
			UNIQUE_LOGERROR("Illegal index range for querying used vertices");
			return false;
		}

		minVertex = M_MAX_UNSIGNED;
		unsigned maxVertex = 0;

		if (GetStride() == sizeof(unsigned))
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
