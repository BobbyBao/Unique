#pragma once
#include "../../Core/Object.h"
#include <LLGL/Buffer.h>
#include "../GfxObject.h"
#include "../../Container/ByteArray.h"

namespace Unique
{
	using BufferFlags = LLGL::BufferFlags;

	class GraphicsBuffer : public TGfxObject<Object, LLGL::Buffer>
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer();
		~GraphicsBuffer();

		bool IsDynamic() const { return (flags_ &  BufferFlags::DynamicUsage) != 0; }
		bool IsShadowed() const { return shadowed_; }

		long flags_ = 0;
		uint elementSize_ = 0;
		uint elementCount_ = 0;
		ByteArray data_;
		bool shadowed_ = false;
		ByteArray shadowData_;
	};


}
