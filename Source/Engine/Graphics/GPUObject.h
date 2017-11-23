#pragma once


namespace Unique
{

	class GPUObject
	{
	public:
		GPUObject();
		~GPUObject();

		virtual void OnCreate();
		virtual void OnDestroy();
		virtual void Sync();
		
		enum class State
		{
			None,
			Created,
			Dead
		};

		State state_;
	};

	template<class T = void>
	class TGPUObject : public GPUObject
	{
	public:
		
		bool IsValid() const { return handle_ != nullptr; }

		virtual void OnCreate()
		{
			GPUObject::OnCreate();
		}

		virtual void OnDestroy()
		{
			GPUObject::OnDestroy();

			if (handle_ != nullptr)
			{
				renderer->Release(*handle_);
			}
		}

		//T& operator() { return *handle_; }
	protected:
		T* handle_ = nullptr;

	};


}
