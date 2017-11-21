#pragma once
#include "../Core/CoreDefs.h"

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
		UNIQUE_ALIGN_DECL(16, byte) m_internal[128];
	};

}

