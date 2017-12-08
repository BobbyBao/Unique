#include "Precompiled.h"
#include "GPUObject.h"
#include "Graphics.h"

namespace Unique
{

	bool GPUObjectBase::Create()
	{
		if (Thread::IsMainThread())
		{
			state_ = State::Creating;
			Graphics::AddCommand([=]()
			{
				ReleaseImpl();
				CreateImpl();
				state_ = State::Created;
			});

		}
		else
		{
			ReleaseImpl();
			CreateImpl();
			state_ = State::Created;
		}

		return true;
	}

	void GPUObjectBase::Release()
	{
		state_ = State::Dying;
		Graphics::AddCommand([=]()
		{
			ReleaseImpl();
			state_ = State::Dead;
		});
	}

	void GPUObjectBase::UpdateBuffer()
	{
	}

	bool GPUObjectBase::CreateImpl()
	{ 
		return true; 
	}

	void GPUObjectBase::ReleaseImpl()
	{
	}
}
