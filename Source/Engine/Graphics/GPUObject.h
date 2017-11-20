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
			Create,
			Living,
			Die,
			Dead
		};

		void* handle_;
		State state_;
	};


}
