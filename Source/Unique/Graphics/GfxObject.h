#pragma once
#include "GraphicsDefs.h"
#include "GraphicsContext.h"

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

	 	virtual bool Create()
		{
			if (Thread::IsMainThread())
			{
				state_ = State::Creating;
				GraphicsContext::AddCommand([=]()
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

		void Release()
		{
			state_ = State::Dying;
			GraphicsContext::AddCommand([=]()
			{
				ReleaseImpl();
				state_ = State::Dead;
			});
		}

		virtual bool CreateImpl()
		{
			return false;
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
