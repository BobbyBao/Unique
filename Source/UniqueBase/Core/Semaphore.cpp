#include "Precompiled.h"
#include "Semaphore.h"

#if UNIQUE_PLATFORM_POSIX
#	include <errno.h>
#	include <pthread.h>
#	include <semaphore.h>
#	include <time.h>
#elif  UNIQUE_PLATFORM_WINDOWS \
	|| UNIQUE_PLATFORM_WINRT   \
	|| UNIQUE_PLATFORM_XBOXONE
#	include <windows.h>
#	include <limits.h>
#	if UNIQUE_PLATFORM_XBOXONE
#		include <synchapi.h>
#	endif // UNIQUE_PLATFORM_XBOXONE
#endif // UNIQUE_PLATFORM_

#ifndef UNIQUE_CONFIG_SEMAPHORE_PTHREAD
#	define UNIQUE_CONFIG_SEMAPHORE_PTHREAD (0 \
			|| UNIQUE_PLATFORM_OSX            \
			|| UNIQUE_PLATFORM_IOS            \
			)
#endif // UNIQUE_CONFIG_SEMAPHORE_PTHREAD

namespace Unique
{
	struct SemaphoreInternal
	{
#if UNIQUE_PLATFORM_POSIX
#	if UNIQUE_CONFIG_SEMAPHORE_PTHREAD
		pthread_mutex_t m_mutex;
		pthread_cond_t m_cond;
		int m_count;
#	else
		sem_t m_handle;
#	endif // UNIQUE_CONFIG_SEMAPHORE_PTHREAD
#elif  UNIQUE_PLATFORM_WINDOWS \
	|| UNIQUE_PLATFORM_WINRT   \
	|| UNIQUE_PLATFORM_XBOXONE
		HANDLE m_handle;
#endif // UNIQUE_PLATFORM_
	};

#if UNIQUE_PLATFORM_POSIX

	uint64_t toNs(const timespec& _ts)
	{
		return _ts.tv_sec*UINT64_C(1000000000) + _ts.tv_nsec;
	}

	void toTimespecNs(timespec& _ts, uint64_t _nsecs)
	{
		_ts.tv_sec  = _nsecs/UINT64_C(1000000000);
		_ts.tv_nsec = _nsecs%UINT64_C(1000000000);
	}

	void toTimespecMs(timespec& _ts, int _msecs)
	{
		toTimespecNs(_ts, _msecs*1000000);
	}

	void add(timespec& _ts, int _msecs)
	{
		uint64_t ns = toNs(_ts);
		toTimespecNs(_ts, ns + _msecs*1000000);
	}

#	if UNIQUE_CONFIG_SEMAPHORE_PTHREAD
	Semaphore::Semaphore()
	{
		UNIQUE_STATIC_ASSERT(sizeof(SemaphoreInternal) <= sizeof(m_internal) );

		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;
		si->m_count = 0;

		int result;

		result = pthread_mutex_init(&si->m_mutex, NULL);
		UNIQUE_CHECK(0 == result, "pthread_mutex_init %d", result);

		result = pthread_cond_init(&si->m_cond, NULL);
		UNIQUE_CHECK(0 == result, "pthread_cond_init %d", result);

		UNIQUE_UNUSED(result);
	}

	Semaphore::~Semaphore()
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		int result;
		result = pthread_cond_destroy(&si->m_cond);
		UNIQUE_CHECK(0 == result, "pthread_cond_destroy %d", result);

		result = pthread_mutex_destroy(&si->m_mutex);
		UNIQUE_CHECK(0 == result, "pthread_mutex_destroy %d", result);

		UNIQUE_UNUSED(result);
	}

	void Semaphore::post(uint _count)
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		int result = pthread_mutex_lock(&si->m_mutex);
		UNIQUE_CHECK(0 == result, "pthread_mutex_lock %d", result);

		for (uint ii = 0; ii < _count; ++ii)
		{
			result = pthread_cond_signal(&si->m_cond);
			UNIQUE_CHECK(0 == result, "pthread_cond_signal %d", result);
		}

		si->m_count += _count;

		result = pthread_mutex_unlock(&si->m_mutex);
		UNIQUE_CHECK(0 == result, "pthread_mutex_unlock %d", result);

		UNIQUE_UNUSED(result);
	}

	bool Semaphore::wait(int _msecs)
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		int result = pthread_mutex_lock(&si->m_mutex);
		UNIQUE_CHECK(0 == result, "pthread_mutex_lock %d", result);

#		if UNIQUE_PLATFORM_OSX
		UNIQUE_UNUSED(_msecs);
		UNIQUE_CHECK(-1 == _msecs, "NaCl and OSX don't support pthread_cond_timedwait at this moment.");
		while (0 == result
		&&     0 >= si->m_count)
		{
			result = pthread_cond_wait(&si->m_cond, &si->m_mutex);
		}
#		elif UNIQUE_PLATFORM_IOS
		if (-1 == _msecs)
		{
			while (0 == result
			&&     0 >= si->m_count)
			{
				result = pthread_cond_wait(&si->m_cond, &si->m_mutex);
			}
		}
		else
		{
			timespec ts;
			toTimespecMs(ts, _msecs);

			while (0 == result
			&&     0 >= si->m_count)
			{
				result = pthread_cond_timedwait_relative_np(&si->m_cond, &si->m_mutex, &ts);
			}
		}
#		else
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		add(ts, _msecs);

		while (0 == result
		&&     0 >= si->m_count)
		{
			result = pthread_cond_timedwait(&si->m_cond, &si->m_mutex, &ts);
		}
#		endif // UNIQUE_PLATFORM_
		bool ok = 0 == result;

		if (ok)
		{
			--si->m_count;
		}

		result = pthread_mutex_unlock(&si->m_mutex);
		UNIQUE_CHECK(0 == result, "pthread_mutex_unlock %d", result);

		UNIQUE_UNUSED(result);

		return ok;
	}

#	else

	Semaphore::Semaphore()
	{
		UNIQUE_STATIC_ASSERT(sizeof(SemaphoreInternal) <= sizeof(m_internal) );

		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		int result = sem_init(&si->m_handle, 0, 0);
		UNIQUE_CHECK(0 == result, "sem_init failed. errno %d", errno);
		UNIQUE_UNUSED(result);
	}

	Semaphore::~Semaphore()
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		int result = sem_destroy(&si->m_handle);
		UNIQUE_CHECK(0 == result, "sem_destroy failed. errno %d", errno);
		UNIQUE_UNUSED(result);
	}

	void Semaphore::post(uint _count)
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		int result;
		for (uint ii = 0; ii < _count; ++ii)
		{
			result = sem_post(&si->m_handle);
			UNIQUE_CHECK(0 == result, "sem_post failed. errno %d", errno);
		}
		UNIQUE_UNUSED(result);
	}

	bool Semaphore::wait(int _msecs)
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

#		if UNIQUE_PLATFORM_OSX
		UNIQUE_CHECK(-1 == _msecs, "NaCl and OSX don't support sem_timedwait at this moment."); UNIQUE_UNUSED(_msecs);
		return 0 == sem_wait(&si->m_handle);
#		else
		if (0 > _msecs)
		{
			int result;
			do
			{
				result = sem_wait(&si->m_handle);
			} // keep waiting when interrupted by a signal handler...
			while (-1 == result && EINTR == errno);
			UNIQUE_CHECK(0 == result, "sem_wait failed. errno %d", errno);
			return 0 == result;
		}

		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		add(ts, _msecs);
		return 0 == sem_timedwait(&si->m_handle, &ts);
#		endif // UNIQUE_PLATFORM_
	}
#	endif // UNIQUE_CONFIG_SEMAPHORE_PTHREAD

#elif  UNIQUE_PLATFORM_WINDOWS \
	|| UNIQUE_PLATFORM_WINRT   \
	|| UNIQUE_PLATFORM_XBOXONE

	Semaphore::Semaphore()
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

#if UNIQUE_PLATFORM_XBOXONE || UNIQUE_PLATFORM_WINRT
		si->m_handle = CreateSemaphoreExW(NULL, 0, LONG_MAX, NULL, 0, SEMAPHORE_ALL_ACCESS);
#else
		si->m_handle = CreateSemaphoreA(NULL, 0, LONG_MAX, NULL);
#endif
	//	UNIQUE_CHECK(NULL != si->m_handle, "Failed to create Semaphore!");
	}

	Semaphore::~Semaphore()
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		CloseHandle(si->m_handle);
	}

	void Semaphore::post(uint _count)
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		ReleaseSemaphore(si->m_handle, _count, NULL);
	}

	bool Semaphore::wait(int _msecs)
	{
		SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

		DWORD milliseconds = (0 > _msecs) ? INFINITE : _msecs;
#if UNIQUE_PLATFORM_XBOXONE || UNIQUE_PLATFORM_WINRT
		return WAIT_OBJECT_0 == WaitForSingleObjectEx(si->m_handle, milliseconds, FALSE);
#else
		return WAIT_OBJECT_0 == WaitForSingleObject(si->m_handle, milliseconds);
#endif
	}
#endif

}
