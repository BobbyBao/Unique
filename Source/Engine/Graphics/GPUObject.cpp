#include "Precompiled.h"
#include "GPUObject.h"

namespace Unique
{
	Vector<GPUObject*> gpuObjects_;

	GPUObject::GPUObject() : state_(State::None)
	{
	}


	GPUObject::~GPUObject()
	{
	}
	
	void GPUObject::OnCreate()
	{
		gpuObjects_.push_back(this);
	}
		
	void GPUObject::OnDestroy()
	{
		gpuObjects_.remove_swap(this);
	}	
	
	void GPUObject::Sync()
	{
	}
}
