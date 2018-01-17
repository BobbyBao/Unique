#pragma once
#include "Core/Object.h"
#include <Buffer.h>
#include "GPUObject.h"

namespace Unique
{
	using BufferDesc = Diligent::BufferDesc;
	using MapType = Diligent::MAP_TYPE;
	using MapFlags = Diligent::MAP_FLAGS;

	class UNIQUE_API GraphicsBuffer : public Object, public GPUObject
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer(uint flags = Diligent::BIND_NONE);
		~GraphicsBuffer();

		template<class T>
		bool Create(Vector<T>&& data, Usage usage = Usage::USAGE_STATIC, uint flags = 0)
		{
			return Create(std::move(reinterpret_cast<ByteArray&&>(data)), sizeof(T), usage, flags);
		}

		template<class T>
		bool Create(const T& data, Usage usage = Usage::USAGE_STATIC, uint flags = 0)
		{
			return Create(1, sizeof(T), usage, flags, (void*)&data);
		}

		/// Set size, vertex elements and dynamic mode. Previous data will be lost.
		bool Create(uint elementCount, uint elementSize, Usage usage, uint flags, void* data);
		
		bool Create(ByteArray&& data, uint elementSize, Usage usage = USAGE_STATIC, uint flags = 0);

		bool SetData(const void* data);
		/// Set a data range in the buffer. Optionally discard data outside the range.
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);

		inline bool IsDynamic() const { return usage_ == USAGE_DYNAMIC; }
		
		inline uint GetSizeInBytes() const { return sizeInBytes_; }
		
		inline uint GetCount() const { return sizeInBytes_ == 0 ? 0 : sizeInBytes_ / stride_; }
		
		inline uint GetStride() const { return stride_; }

		virtual void UpdateBuffer();

		void* Lock(uint lockStart = 0, uint lockCount = -1);
		void Unlock();

		void* Map(uint mapFlags = Diligent::MAP_FLAG_DISCARD);
		void UnMap();

		inline char* GetShadowData() { return data_[0].data(); }
	protected:
		/// Create buffer.
		virtual bool CreateImpl();
		uint bindFlags_;
		Usage usage_;
		uint stride_;
		uint sizeInBytes_;
		uint cpuAccessFlags_;
		ByteArray data_[2];
		uint lockStart_[2];
		uint lockCount_[2];
		uint mapFlags_;
	};

	class UNIQUE_API IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		IndexBuffer() : GraphicsBuffer(Diligent::BIND_INDEX_BUFFER) {}

		IndexBuffer(Vector<uint>&& data, Usage usage = USAGE_STATIC, uint flags = 0) 
			: GraphicsBuffer(Diligent::BIND_INDEX_BUFFER)
		{
			Create(std::move(data), usage, flags);
		}

		IndexBuffer(Vector<ushort>&& data, Usage usage = USAGE_STATIC, uint flags = 0) 
			: GraphicsBuffer(Diligent::BIND_INDEX_BUFFER)
		{
			Create(std::move(data), usage, flags);
		}

		/// Set size and vertex elements and dynamic mode. Previous data will be lost.
		bool SetSize(unsigned indexCount, bool largeIndices, bool dynamic = false);

		inline uint GetIndexSize() const { return GetStride(); }

		inline uint GetIndexCount() const { return GetCount(); }
	};

	class UNIQUE_API UniformBuffer : public GraphicsBuffer
	{
		uRTTI(UniformBuffer, GraphicsBuffer)
	public:
		UniformBuffer() : GraphicsBuffer(Diligent::BIND_UNIFORM_BUFFER) {}

		template<class T>
		UniformBuffer(const T& data, Usage usage = USAGE_DYNAMIC, uint flags = Diligent::CPU_ACCESS_WRITE)
			: GraphicsBuffer(Diligent::BIND_UNIFORM_BUFFER)
		{
			Create(data, usage, flags);
		}

		template<class T>
		void SetData(const T& data)
		{
			GraphicsBuffer::SetData((void*)&data);
		}
	};
}
