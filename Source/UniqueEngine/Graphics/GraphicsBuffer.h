#pragma once
#include "Core/Object.h"
#include <Buffer.h>
#include "GPUObject.h"

namespace Unique
{
	class UNIQUE_API GraphicsBuffer : public Object, public GPUObject<IBuffer>
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer(uint flags = BIND_NONE);
		~GraphicsBuffer();

		template<class T>
		bool Create(const Vector<T>& data, Usage usage = Usage::USAGE_STATIC)
		{
			return Create((uint)data.size(), sizeof(T), usage, (void*)data.data());
		}

		template<class T>
		bool Create(const T& data, Usage usage = Usage::USAGE_STATIC)
		{
			return Create(1, sizeof(T), usage, (void*)&data);
		}

		/// Set size, vertex elements and dynamic mode. Previous data will be lost.
		bool Create(uint elementCount, uint elementSize, Usage usage = Usage::USAGE_STATIC, void* data = 0);
		bool Create(ByteArray&& data, uint elementSize, Usage usage = Usage::USAGE_STATIC);

		bool SetData(const void* data);
		/// Set a data range in the buffer. Optionally discard data outside the range.
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);

		inline bool IsDynamic() const { return desc_.Usage == USAGE::USAGE_DYNAMIC; }
		inline uint GetSizeInBytes() const { return desc_.uiSizeInBytes; }
		inline uint GetCount() const { return desc_.uiSizeInBytes / desc_.ElementByteStride; }
		inline uint GetStride() const { return desc_.ElementByteStride; }

		virtual void UpdateBuffer();
	protected:
		/// Create buffer.
		virtual bool CreateImpl();

		BufferDesc desc_;
		ByteArray data_;
	};

	class UNIQUE_API IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		IndexBuffer() : GraphicsBuffer(BIND_INDEX_BUFFER) {}
	};

	class UNIQUE_API ConsterBuffer : public GraphicsBuffer
	{
		uRTTI(ConsterBuffer, GraphicsBuffer)
	public:
		ConsterBuffer() : GraphicsBuffer(BIND_UNIFORM_BUFFER) {}
	};
}
