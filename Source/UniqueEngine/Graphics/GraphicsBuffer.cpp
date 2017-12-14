#include "Precompiled.h"
#include "GraphicsBuffer.h"
#include "Graphics/Graphics.h"
#include <InputLayout.h>

namespace Unique
{
	uEnumTraits
	(
		Usage,
		"STATIC",
		"DEFAULT",
		"DYNAMIC",
		"CPU_ACCESSIBLE"
	)

	uObject(GraphicsBuffer)
	{
		uAttribute("SizeInBytes", desc_.uiSizeInBytes)
		uAttribute("Usage", desc_.Usage)
		uAttribute("CPUAccessFlags", desc_.CPUAccessFlags)
	}
	
	uObject(IndexBuffer)
	{
		uFactory("Graphics")
		uAttribute("ElementByteStride", desc_.ElementByteStride)
		uAttribute("Data", data_)
	}

	uObject(ConstBuffer)
	{
		uFactory("Graphics")
		uAttribute("Data", data_)
	}

	GraphicsBuffer::GraphicsBuffer(uint flags)
	{
		desc_.BindFlags = flags;
	}

	GraphicsBuffer::~GraphicsBuffer()
	{
	}

	bool GraphicsBuffer::Create(uint elementCount, uint elementSize, Usage usage, uint flags, void* data)
	{
		desc_.ElementByteStride = elementSize;
		desc_.uiSizeInBytes = elementCount*elementSize;
		desc_.Usage = usage;
		desc_.CPUAccessFlags = flags;
		data_.resize(desc_.uiSizeInBytes);
		
		if (data)
		{
			std::memcpy(data_.data(), data, desc_.uiSizeInBytes);
		}

		return GPUObject::Create();
	}

	bool GraphicsBuffer::Create(ByteArray&& data, uint elementSize, Usage usage, uint flags)
	{
		desc_.ElementByteStride = elementSize;
		desc_.uiSizeInBytes = (uint)data.size();
		desc_.Usage = usage;
		desc_.CPUAccessFlags = flags;
		data_ = data;

		return GPUObject::Create();
	}

	bool GraphicsBuffer::CreateImpl()
	{
		if (!GetSizeInBytes())
			return false;

		BufferData BuffData;
		BuffData.pData = data_.data();
		BuffData.DataSize = desc_.uiSizeInBytes;
		renderDevice->CreateBuffer(desc_, BuffData, (IBuffer**)&handle_);
		return handle_ != nullptr;
	}

	bool GraphicsBuffer::SetData(const void* data)
	{
		if (desc_.Usage == USAGE_STATIC)
		{
			UNIQUE_LOGERROR("Set data for static buffer");
			return false;
		}

		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for buffer data");
			return false;
		}

		if (GetSizeInBytes() == 0)
		{
			UNIQUE_LOGERROR("Null element size, can not set buffer data");
			return false;
		}

		if (IsDynamic())
		{
			ByteArray& currentData = Graphics::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data(), data, currentData.size());
			dirty_[Graphics::currentContext_] = true; 
			lockStart_[Graphics::currentContext_] = 0;
			lockCount_[Graphics::currentContext_] = (uint)currentData.size();
		}
		else
		{
			std::memcpy(data_.data(), data, data_.size());

			Graphics::AddCommand([=]()
			{			
				IBuffer* buffer = *this;
				void* bufferData = nullptr;
				buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
				memcpy(bufferData, data_.data(), data_.size());
				buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);
			});
		}


		return true;
	}

	bool GraphicsBuffer::SetDataRange(const void* data, unsigned start, unsigned count, bool discard)
	{
		if (desc_.Usage == USAGE_STATIC)
		{
			UNIQUE_LOGERROR("Set data for static buffer");
			return false;
		}

		if (start == 0 && count == GetCount())
			return SetData(data);

		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for buffer data");
			return false;
		}

		if (GetStride() == 0)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set buffer data");
			return false;
		}

		if (start + count > GetCount())
		{
			UNIQUE_LOGERROR("Illegal range for setting new buffer data");
			return false;
		}

		if (!count)
			return true;

		if (IsDynamic())
		{
			ByteArray& currentData = Graphics::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data() + start * GetStride(), data, count * GetStride());
			dirty_[Graphics::currentContext_] = true;
			lockStart_[Graphics::currentContext_] = start;
			lockCount_[Graphics::currentContext_] = count;
		}
		else
		{
			std::memcpy(data_.data() + start * GetStride(), data, count * GetStride());
			
			Graphics::AddCommand([=]()
			{
				IBuffer* buffer = *this; 
				void* bufferData = nullptr;
				buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
				memcpy(bufferData, data_.data() + start * GetStride(), count * GetStride());
				buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);
			});
		}

		return true;
	}

	void GraphicsBuffer::UpdateBuffer()
	{
		if (dirty_[Graphics::GetRenderContext()])
		{
			IBuffer* buffer = *this;
			void* bufferData = nullptr;
			buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
			memcpy(bufferData, data_.data() + lockStart_[Graphics::GetRenderContext()] * GetStride(), 
				lockCount_[Graphics::GetRenderContext()] * GetStride());
			buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);

		}
	}

}
