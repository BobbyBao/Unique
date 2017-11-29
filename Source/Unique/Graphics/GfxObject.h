#pragma once
#include "GraphicsDefs.h"

namespace Unique
{

	extern UPtr<LLGL::RenderSystem> renderer;

	template<class Super, class T = void>
	class TGfxObject : public Super
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

		void Create()
		{
			//	gpuObjects_.push_back(this);

			state_ = State::Creating;
			Subsystem<Graphics>().AddCommand([this]()
			{
				CreateImpl();
				state_ = State::Created;
			});
		}

		void Release()
		{
			//	RemoveSwap(gpuObjects_, this);
			state_ = State::Dying;
			Subsystem<Graphics>().AddCommand([this]()
			{
				ReleaseImpl();
				state_ = State::Dead;
			});
		}

		virtual void ReleaseImpl()
		{
			if (handle_ != nullptr)
			{
				renderer->Release(*handle_);
				handle_ = nullptr;
			}
		}

		operator T&() { return *handle_; }

		State state_ = State::None;
		T* handle_ = nullptr;

	};


}
