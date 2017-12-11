#pragma once
#include "GraphicsDefs.h"

namespace Unique
{
	class GPUObjectBase
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
		//call in render thread
		virtual void UpdateBuffer();

	protected:	
		virtual bool CreateImpl();
		virtual void ReleaseImpl();

		State state_ = State::None;
	};

	template<class T = void>
	class GPUObject : public GPUObjectBase
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

		bool IsValid() const { return handle_ != nullptr; }


		operator T&() { return *handle_; }

		T* GetHandle() const { return handle_; }
	protected:

		virtual void ReleaseImpl()
		{
			if (handle_ != nullptr)
			{
				handle_->Release();
				handle_ = nullptr;
			}
		}

		T* handle_ = nullptr;

		friend class Graphics;
	};


}
