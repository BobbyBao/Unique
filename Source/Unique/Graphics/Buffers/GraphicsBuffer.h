#pragma once
#include "Core/Object.h"
#include <LLGL/Buffer.h>
#include "../GPUObject.h"

namespace Unique
{
	using BufferFlags = LLGL::BufferFlags;

	class UNIQUE_API GraphicsBuffer : public GPUObject<Object, LLGL::Buffer>
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer();
		~GraphicsBuffer();

		bool SetData(const void* data);
		/// Set a data range in the buffer. Optionally discard data outside the range.
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);

		bool CpuRead() const { return (flags_ &  BufferFlags::MapReadAccess) != 0; }
		bool CpuWrite() const { return (flags_ &  BufferFlags::MapWriteAccess) != 0; }
		bool IsDynamic() const { return (flags_ &  BufferFlags::DynamicUsage) != 0; }

		virtual void UpdateBuffer();
	protected:
		uint elementSize_ = 0;
		uint elementCount_ = 0;
		ByteArray data_; 
		ByteArray data1_;
		long flags_ = 0;
	};


}
