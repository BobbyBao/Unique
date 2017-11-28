#pragma once
#include "../../Core/Object.h"
#include <LLGL/Buffer.h>
#include "../GfxObject.h"

namespace Unique
{
	class GraphicsBuffer : public Object, public TGfxObject<LLGL::Buffer>
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer();
		~GraphicsBuffer();

		uint elementSize_ = 0;
		Vector<byte> data_;
		LLGL::BufferDescriptor bufferDesc_;
	};


}
