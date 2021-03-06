#include "Precompiled.h"
#include "GPUObject.h"
#include "Graphics.h"

namespace Unique
{
	Vector<GPUObject*> GPUObject::updateQueue_[2];

	bool GPUObject::Create()
	{
		uCall
		(
			ReleaseImpl();
			CreateImpl();
		);

		return true;
	}

	void GPUObject::Release()
	{
		uPost
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
			GetSubsystem<Graphics>().ReleaseDeviceObject(deviceObject_);
		//	deviceObject_->Release();
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
