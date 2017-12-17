#pragma once
#include "GraphicsDefs.h"

namespace Unique
{
	class GPUObject
	{
	public:
		enum class State
		{
			None,
			Creating,
			Created,
			Dying,
			Dead
		};

		virtual bool Create();
		virtual void Release();

		bool IsValid() const { return deviceObject_ != nullptr; }

		template<class T>
		operator T*() { return (T*)deviceObject_; }
		
	protected:	
		virtual bool CreateImpl();
		virtual void ReleaseImpl();
		virtual void UpdateBuffer();
		void MarkDirty();
		static void UpdateBuffers();

		State state_ = State::None;
		IDeviceObject* deviceObject_ = nullptr;
		static Vector<GPUObject*> updateQueue_[2];
		friend class Graphics;
	};
	

}
