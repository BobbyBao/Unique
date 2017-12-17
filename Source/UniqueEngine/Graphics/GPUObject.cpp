#include "Precompiled.h"
#include "GPUObject.h"
#include "Graphics.h"

namespace Unique
{

	bool GPUObject::Create()
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

	void GPUObject::Release()
	{
		state_ = State::Dying;
		Graphics::AddCommand([=]()
		{
			ReleaseImpl();
			state_ = State::Dead;
		});
	}

	void GPUObject::UpdateBuffer()
	{
	}

	bool GPUObject::CreateImpl()
	{ 
		return true; 
	}

	void GPUObject::ReleaseImpl()
	{
		if (deviceObject_ != nullptr)
		{
			deviceObject_->Release();
			deviceObject_ = nullptr;
		}
	}
}
