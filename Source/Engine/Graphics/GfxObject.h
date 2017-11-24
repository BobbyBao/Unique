#pragma once
#include "GraphicsDefs.h"

namespace Unique
{

	class GfxObject
	{
	public:
		GfxObject();
		~GfxObject();

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void OnSync();
		
		enum class State
		{
			None,
			Created,
			Dead
		};

		State state_;
	};

	template<class T = void>
	class TGfxObject : public GfxObject
	{
	public:
		
		bool IsValid() const { return handle_ != nullptr; }

		virtual void OnCreate()
		{
			GfxObject::OnCreate();
		}

		virtual void OnDestroy()
		{
			GfxObject::OnDestroy();

			if (handle_ != nullptr)
			{
				renderer->Release(*handle_);
			}
		}

		operator T&() { return *handle_; }

		T* handle_ = nullptr;

	};


}
