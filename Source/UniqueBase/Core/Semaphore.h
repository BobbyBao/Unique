#pragma once
#include "../Core/CoreDefs.h"
//#define USE_STL

#ifdef USE_STL

#include <mutex>
#include <condition_variable>

#endif

namespace Unique
{
	///
	class Semaphore
	{
	public:
		///
		Semaphore();

		///
		~Semaphore();

		///
		void post(uint _count = 1);

		///
		bool wait(int _msecs = -1);

	private:
#ifdef USE_STL

		int count = 1;
		int wakeups = 0;
		std::mutex mutex;
		std::condition_variable condition;

#else
		UNIQUE_ALIGN_DECL(16, byte) m_internal[128];
#endif
	};


}

