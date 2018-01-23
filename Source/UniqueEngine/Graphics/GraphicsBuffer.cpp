#include "Precompiled.h"
#include "GraphicsBuffer.h"
#include "Graphics/Graphics.h"


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
		uAttribute("Usage", usage_)
		uAttribute("SizeInBytes", sizeInBytes_)
		uAttribute("CPUAccessFlags", cpuAccessFlags_)
	}
	
	uObject(IndexBuffer)
	{
		uFactory("Graphics")
		uAttribute("ElementByteStride", stride_)
		uAttribute("Data", data_[0])
	}

	uObject(UniformBuffer)
	{
		uFactory("Graphics")
		uAttribute("Data", data_[0])
	}

	GraphicsBuffer::GraphicsBuffer(uint flags) : cpuAccessFlags_(0)
	{
		bindFlags_ = flags;
	}

	GraphicsBuffer::~GraphicsBuffer()
	{
	}

	bool GraphicsBuffer::Create(uint elementCount, uint elementSize, Usage usage, uint flags, void* data)
	{
		stride_ = elementSize;
		sizeInBytes_ = elementCount*elementSize;
		usage_ = usage;
		cpuAccessFlags_ = flags;

		data_[0].resize(sizeInBytes_);

		if (IsDynamic())
		{
			data_[1].resize(sizeInBytes_);
		}
		
		if (data)
		{
			auto& currentData = IsDynamic() ? MainContext(data_) : data_[0];
			std::memcpy(currentData.data(), data, sizeInBytes_);
		}

		return GPUObject::Create();
	}

	bool GraphicsBuffer::Create(ByteArray&& data, uint elementSize, Usage usage, uint flags)
	{
		stride_ = elementSize;
		sizeInBytes_ = (uint)data.size();
		usage_ = usage;
		cpuAccessFlags_ = flags;

		auto& currentData = IsDynamic() ? MainContext(data_) : data_[0];
		currentData = data;

		return GPUObject::Create();
	}

	bool GraphicsBuffer::CreateImpl()
	{
		if (!GetSizeInBytes())
			return false;
		
		auto& graphics = GetSubsystem<Graphics>();
		auto& currentData = IsDynamic() ? RenderContext(data_) : data_[0];
		graphics.CreateBuffer(*this, currentData);
		return deviceObject_ != nullptr;
	}

	bool GraphicsBuffer::SetData(const void* data)
	{
		if (usage_ == USAGE_STATIC)
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
				auto& graphics = GetSubsystem<Graphics>();
				void* bufferData = graphics.Map(this);
				memcpy(bufferData, data_[0].data(), data_[0].size());
				graphics.Unmap(this);
			);
		}


		return true;
	}

	bool GraphicsBuffer::SetDataRange(const void* data, unsigned start, unsigned count, bool discard)
	{
		if (usage_ == USAGE_STATIC)
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
			MainContext(lockStart_) = start;
			MainContext(lockCount_) = count;
		}
		else
		{
			std::memcpy(data_[0].data() + start * GetStride(), data, count * GetStride());
			
			uCall
			(
				auto& graphics = GetSubsystem<Graphics>();
				void* bufferData = graphics.Map(this);
				memcpy(bufferData, data_[0].data() + start * GetStride(), count * GetStride());
				graphics.Unmap(this);
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
				auto& graphics = GetSubsystem<Graphics>();
				void* bufferData = graphics.Map(this);
				memcpy(bufferData, data_[0].data() + start * GetStride(), count * GetStride());
				graphics.Unmap(this);
			);
		}
	}


	void* GraphicsBuffer::Map(MapFlags mapFlags)
	{
		auto& graphics = GetSubsystem<Graphics>();
		void* bufferData = graphics.Map(this, mapFlags);
		mapFlags_ = mapFlags;
		return bufferData;
	}

	void GraphicsBuffer::UnMap()
	{
		auto& graphics = GetSubsystem<Graphics>();
		graphics.Unmap(this, mapFlags_);
	}

	void GraphicsBuffer::UpdateBuffer()
	{
		auto& graphics = GetSubsystem<Graphics>();
		void* bufferData = graphics.Map(this);
		memcpy(bufferData, RenderContext(data_).data() + RenderContext(lockStart_) * GetStride(),
			RenderContext(lockCount_) * GetStride());
		graphics.Unmap(this);
	}

	bool IndexBuffer::SetSize(unsigned indexCount, bool largeIndices, bool dynamic)
	{
		Usage usage = dynamic ? USAGE_DYNAMIC : USAGE_STATIC;
		uint flags = (dynamic ? Diligent::CPU_ACCESS_WRITE : 0);
		return GraphicsBuffer::Create(indexCount, largeIndices ? 4 : 2, usage, flags, nullptr);
	}
}
