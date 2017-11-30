#pragma once

#include "GraphicsBuffer.h"

namespace Unique
{
	class IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		IndexBuffer();
		~IndexBuffer();

		IndexFormat indexFormat_;
	};

}