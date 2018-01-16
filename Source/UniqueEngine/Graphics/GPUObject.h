#pragma once
#include "GraphicsDefs.h"

namespace Unique
{
	class UNIQUE_API GPUObject
	{
	public:
		virtual bool Create();
		virtual void Release();

		bool IsValid() const { return deviceObject_ != nullptr; }

		template<class T>
		operator T*() { return (T*)deviceObject_; }
		template<class T>
		operator T**() { return (T**)&deviceObject_; }
		template<class T>
		operator const T**()const { return (T**)&deviceObject_; }
	protected:	
		virtual bool CreateImpl();
		virtual void ReleaseImpl();
		virtual void UpdateBuffer();
		void MarkDirty();

		static void UpdateBuffers();
		IDeviceObject* deviceObject_ = nullptr;
		static Vector<GPUObject*> updateQueue_[2];
		friend class Graphics;
	};
	

}
