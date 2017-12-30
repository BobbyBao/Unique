#include "Precompiled.h"
#include "GraphicsBuffer.h"
#include "Graphics/Graphics.h"
#include <InputLayout.h>

using namespace Diligent;

namespace Unique
{
	uEnum
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
		uAttribute("Data", data_[0])
	}

	uObject(UniformBuffer)
	{
		uFactory("Graphics")
		uAttribute("Data", data_[0])
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

		data_[0].resize(desc_.uiSizeInBytes);

		if (IsDynamic())
		{
			data_[1].resize(desc_.uiSizeInBytes);
		}
		
		if (data)
		{
			auto& currentData = IsDynamic() ? MainContext(data_) : data_[0];
			std::memcpy(currentData.data(), data, desc_.uiSizeInBytes);
		}

		return GPUObject::Create();
	}

	bool GraphicsBuffer::Create(ByteArray&& data, uint elementSize, Usage usage, uint flags)
	{
		desc_.ElementByteStride = elementSize;
		desc_.uiSizeInBytes = (uint)data.size();
		desc_.Usage = usage;
		desc_.CPUAccessFlags = flags;

		auto& currentData = IsDynamic() ? MainContext(data_) : data_[0];
		currentData = data;

		return GPUObject::Create();
	}

	bool GraphicsBuffer::CreateImpl()
	{
		if (!GetSizeInBytes())
			return false;

		auto& currentData = IsDynamic() ? MainContext(data_) : data_[0];
		BufferData BuffData;
		BuffData.pData = currentData.data();
		BuffData.DataSize = desc_.uiSizeInBytes;
		renderDevice->CreateBuffer(desc_, BuffData, (IBuffer**)&deviceObject_);
		return deviceObject_ != nullptr;
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
			ByteArray& currentData = MainContext(data_);
			std::memcpy(currentData.data(), data, currentData.size());
			MarkDirty();
			MainContext(lockStart_) = 0;
			MainContext(lockCount_) = (uint)currentData.size();
		}
		else
		{
			std::memcpy(data_[0].data(), data, data_[0].size());

			uCall
			(
				IBuffer* buffer = *this;
				void* bufferData = nullptr;
				buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
				memcpy(bufferData, data_[0].data(), data_[0].size());
				buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);
			);
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
			ByteArray& currentData = MainContext(data_);
			std::memcpy(currentData.data() + start * GetStride(), data, count * GetStride());
			MarkDirty();
			lockStart_[Graphics::currentContext_] = start;
			lockCount_[Graphics::currentContext_] = count;
		}
		else
		{
			std::memcpy(data_[0].data() + start * GetStride(), data, count * GetStride());
			
			uCall
			(
				IBuffer* buffer = *this; 
				void* bufferData = nullptr;
				buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
				memcpy(bufferData, data_[0].data() + start * GetStride(), count * GetStride());
				buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);
			);
		}

		return true;
	}

	void* GraphicsBuffer::Lock(uint lockStart, uint lockCount)
	{
		auto& currentData = IsDynamic() ? MainContext(data_) : data_[0];
		
		if (IsDynamic())
		{
			MainContext(lockStart_) = lockStart;
			MainContext(lockCount_) = std::min(lockCount, GetCount());
		}
		else
		{
			lockStart_[0] = lockStart;
			lockCount_[0] = std::min(lockCount, GetCount());
		}

		return currentData.data();
	}

	void GraphicsBuffer::Unlock()
	{
		if (IsDynamic())
		{
			MarkDirty();
		}
		else
		{
			uint start = lockStart_[0];
			uint count = lockCount_[0];
			uCall
			(
				IBuffer* buffer = *this;
				void* bufferData = nullptr;
				buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
				memcpy(bufferData, data_[0].data() + start * GetStride(), count * GetStride());
				buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);
			);
		}
	}


	void* GraphicsBuffer::Map(uint mapFlags)
	{
		IBuffer* buffer = *this;
		void* bufferData = nullptr;
		buffer->Map(deviceContext, MAP_WRITE, mapFlags, bufferData);
		mapFlags_ = mapFlags;
		return bufferData;
	}

	void GraphicsBuffer::UnMap()
	{
		IBuffer* buffer = *this;
		buffer->Unmap(deviceContext, MAP_WRITE, mapFlags_);
	}

	void GraphicsBuffer::UpdateBuffer()
	{
		IBuffer* buffer = *this;
		void* bufferData = nullptr;
		buffer->Map(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD, bufferData);
		memcpy(bufferData, RenderContext(data_).data() + RenderContext(lockStart_) * GetStride(),
			RenderContext(lockCount_) * GetStride());
		buffer->Unmap(deviceContext, MAP_WRITE, MAP_FLAG_DISCARD);
	}

}
