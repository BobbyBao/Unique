#pragma once

#include "GraphicsBuffer.h"

namespace Unique
{
	class StorageBuffer : public GraphicsBuffer
	{
		uRTTI(StorageBuffer, GraphicsBuffer)
	public:
		StorageBuffer();
		~StorageBuffer();

	//	StorageBufferDescriptor desc_;
	};

}
