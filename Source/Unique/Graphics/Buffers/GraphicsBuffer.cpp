#include "Precompiled.h"
#include "GraphicsBuffer.h"

namespace Unique
{
	uObject(GraphicsBuffer)
	{
		uAttribute("ElementSize", elementSize_)
		uAttribute("ElementCount", elementCount_)
	}

	GraphicsBuffer::GraphicsBuffer()
	{
	}


	GraphicsBuffer::~GraphicsBuffer()
	{
	}

}
