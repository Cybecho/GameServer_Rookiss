#pragma once

#define OUT

/*---------------
	  Lock 관련 매크로
---------------*/

#define USE_MANY_LOCKS(count)		Lock _locks[count];
#define USE_LOCK								USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)				ReadLockGuard _readLockGuard##idx(_locks[idx]);
#define READ_LOCK							READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)				WriteLockGuard _writeLockGuard##idx(_locks[idx]);
#define WRITE_LOCK							WRITE_LOCK_IDX(0)


/*---------------
	  Crash 관련 매크로
---------------*/

#define CRASH(cause)						\
{														\
	uint32* crash = nullptr;					\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{													\
	if (!(expr))									\
	{												\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);			\
	}												\
}