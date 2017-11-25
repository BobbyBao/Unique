#include "Precompiled.h"
#include "GfxObject.h"

namespace Unique
{
	Vector<GfxObject*> gpuObjects_;

	GfxObject::GfxObject() : state_(State::None)
	{
	}

	GfxObject::~GfxObject()
	{
	}
	
	void GfxObject::OnCreate()
	{
		gpuObjects_.push_back(this);
		state_ = State::Created;
	}
		
	void GfxObject::OnDestroy()
	{
		state_ = State::Dead;
		RemoveSwap(gpuObjects_, this);
	}	
	
	void GfxObject::OnSync()
	{
	}
}
