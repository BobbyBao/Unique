#include "Precompiled.h"
#include "GPUObject.h"
#include "Graphics.h"

namespace Unique
{
	Vector<GPUObject*> GPUObject::updateQueue_[2];
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

	void GPUObject::MarkDirty()
	{
		auto& renderContext = updateQueue_[Graphics::currentContext_];
		renderContext.push_back(this);
	}

	void GPUObject::UpdateBuffers()
	{
		auto& renderContext = updateQueue_[Graphics::GetRenderContext()];
		for (auto buffer : renderContext)
		{
			buffer->UpdateBuffer();
		}

		renderContext.clear();
	}
}
