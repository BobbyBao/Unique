#pragma once
#include "GraphicsBuffer.h"

namespace Unique
{
	class VertexBuffer : public GraphicsBuffer
	{
		uRTTI(VertexBuffer, GraphicsBuffer)
	public:
		VertexBuffer();
		~VertexBuffer();
	};


}
