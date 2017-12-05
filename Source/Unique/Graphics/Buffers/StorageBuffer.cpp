#include "Precompiled.h"
#include "StorageBuffer.h"

namespace Unique
{
	uObject(StorageBuffer)
	{
		uFactory("Graphics")
		uAttribute("Data", data_)
	}

	StorageBuffer::StorageBuffer()
	{
	}


	StorageBuffer::~StorageBuffer()
	{
	}

}
