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

		LLGL::BufferDescriptor bufferDesc_;
	};


}
