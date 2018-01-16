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
			uCall
			(
				ReleaseImpl();
				CreateImpl();
			);

		}
		else
		{
			ReleaseImpl();
			CreateImpl();
		}

		return true;
	}

	void GPUObject::Release()
	{
		uCall
		(
			ReleaseImpl();
		);
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
		auto& renderContext = MainContext(updateQueue_);
		renderContext.push_back(this);
	}

	void GPUObject::UpdateBuffers()
	{
		auto& buffers = RenderContext(updateQueue_);
		if (!buffers.empty())
		{
			for (auto buffer : buffers)
			{
				buffer->UpdateBuffer();
			}

			buffers.clear();
		}
	}
}
