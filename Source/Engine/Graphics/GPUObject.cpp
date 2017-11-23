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
		state_ = State::Created;
	}
		
	void GPUObject::OnDestroy()
	{
		state_ = State::Dead;
		RemoveSwap(gpuObjects_, this);
	}	
	
	void GPUObject::Sync()
	{
	}
}
