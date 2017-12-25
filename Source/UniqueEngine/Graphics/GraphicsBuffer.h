#pragma once
#include "Core/Object.h"
#include <Buffer.h>
#include "GPUObject.h"

namespace Unique
{
	class UNIQUE_API GraphicsBuffer : public Object, public GPUObject
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer(uint flags = BIND_NONE);
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

		inline bool IsDynamic() const { return desc_.Usage == USAGE::USAGE_DYNAMIC; }
		
		inline uint GetSizeInBytes() const { return desc_.uiSizeInBytes; }
		
		inline uint GetCount() const { return desc_.uiSizeInBytes / desc_.ElementByteStride; }
		
		inline uint GetStride() const { return desc_.ElementByteStride; }

		virtual void UpdateBuffer();

		void* Lock(uint lockStart = 0, uint lockCount = -1);
		void Unlock();
	protected:
		/// Create buffer.
		virtual bool CreateImpl();

		BufferDesc desc_;
		ByteArray data_[2];
		uint lockStart_[2];
		uint lockCount_[2];
	};

	class UNIQUE_API IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		IndexBuffer() : GraphicsBuffer(BIND_INDEX_BUFFER) {}

		IndexBuffer(Vector<uint>&& data, Usage usage = Usage::USAGE_STATIC, uint flags = 0) : GraphicsBuffer(BIND_INDEX_BUFFER)
		{
			Create(data, usage, flags);
		}

		IndexBuffer(Vector<ushort>&& data, Usage usage = Usage::USAGE_STATIC, uint flags = 0) : GraphicsBuffer(BIND_INDEX_BUFFER)
		{
			Create(data, usage, flags);
		}
	};

	class UNIQUE_API UniformBuffer : public GraphicsBuffer
	{
		uRTTI(UniformBuffer, GraphicsBuffer)
	public:
		UniformBuffer() : GraphicsBuffer(BIND_UNIFORM_BUFFER) {}

		template<class T>
		UniformBuffer(const T& data, Usage usage = Usage::USAGE_DYNAMIC, uint flags = CPU_ACCESS_WRITE) : GraphicsBuffer(BIND_UNIFORM_BUFFER)
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
