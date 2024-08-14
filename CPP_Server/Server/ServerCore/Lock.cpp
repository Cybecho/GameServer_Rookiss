#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

//! 아무도 Read중이 아닐때 Write 가능!
void Lock::WriteLock()
{
	//? 중요한건, ReadLock이 하나라도 있으면 WriteLock은 가질 수 없음
	// ? 즉, 쓰기 위해선 아무도 읽지 않고 있어야함!
	// Write를 아무도 소유 및 공유하고 있지 않을때, 경합해서 소유권을 얻는다
	
	//~ 만약, 동일한 스레드가 이미 WRITE LOCK을 소유하고 있다면?
	//~ 현재 WRITE COUNT MASK 영역에 있는 비트를 하위 16비트 부분으로 옮기고
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_COUNT_MASK) >> 16;
	if (LThreadId == lockThreadId) //~ 내 ID와 지금 WriteLock을 소유중인 Lock과 동일한 스레드야?
	{
		_writeCount++;
		return;
	}

	//~ [WWWWWWWW] [WWWWWWWW]  영역에 내 lock번호를 기입하면 내꺼가 됨
	//~ 그럼, 내 스레드 아이디인 LTreadId를 [W]영역에 16비트를 적어서 옮겨줌
	const int64 beginTick = GetTickCount64();										// 시작 시간
	const uint32 desired = ((LThreadId << 16) & WRITE_COUNT_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{	//? 5000번동안 스핀락 삥삥돌며 WRITE LOCK 얻어보자~
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{	//? 여기까지 들어왔으면, 이제 WRITE LOCK을 얻은거임
				_writeCount++;
				return;
			}
		}
		// 대기 시간이 너무 오래걸린다면 그냥 프로그램 크래시내자! 
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK TIMEOUT ! ! !");

		this_thread::yield();	//? 5000번 스핀락 돌았는데도 못얻었으면, 다른 스레드에게 양보하고 다시 시도해보자~
	}
}

void Lock::WriteUnlock()
{
	//~ 아! 어처피 Write는 아무도 Read를 하고있지않을때 가능하니까, 
	//~ 내가 Write하지 않는다는것은,
	//~ 결국 Write비트와 Read비트 둘 다 0이 되는 상황이겠구나!

	if((_lockFlag.load() & READ_COUNT_MASK) != 0) // 읽는 스레드가 하나라도 있으면 WriteLock을 해제할 수 없다
		CRASH("READ LOCK IS ALREADY EXIST ! ! !");

	const int32 lockCount = --_writeCount; // 내가 소유한 WriteLock을 하나 줄인다
	if (lockCount == 0)								// 내가 소유한 WriteLock이 없다면
		_lockFlag.store(EMPTY_FLAG);			// 내가 소유한 lockFlag를 Empty로 만들어준다
		
}

//! 아무도 Write중이 아닐때 Read 가능!
void Lock::ReadLock()
{
	//~ 이미 WRITE LOCK을 소유하고 있는 상태에서 READ를 하려고한다면?
	//~ 현재 WRITE COUNT MASK 영역에 있는 비트를 하위 16비트 부분으로 옮기고
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_COUNT_MASK) >> 16;
	if (LThreadId == lockThreadId) //~ 내 ID와 지금 WriteLock을 소유중인 Lock과 동일한 스레드야?
	{
		_lockFlag.fetch_add(1); // 내가 이미 write를 가지고있으면, 다른 스레드는 어처피 못들어오잖아?
		return;
	}

	// 아무도  ReadLock을 소유하고 있지 않을때, 경합해서 소유권을 얻는다
	const int64 beginTick = GetTickCount64();										// 시작 시간
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//~ READ 16비트 영역을 제외하고, WRITE 16비트 영역이 비어있을때 ReadLock을 잡아야함
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected+1))
				return; //~ 위 상황이 맞으면, expected값을 1증가시켜서 READ LOCK을 얻은거임
		}
		// 대기 시간이 너무 오래걸린다면 그냥 프로그램 크래시내자! 
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK TIMEOUT ! ! !");

		this_thread::yield(); // 5000번 스핀락 돌았는데도 못얻었으면, 다른 스레드에게 양보하고 다시 시도해보자~
	}
}

void Lock::ReadUnlock()
{
	//~ fetch_sub 는 후위연산자랑 똑같아서 이전값 뱉는데, 이전값이 이미 0이야? 
	//~ 묘한 상황, 그냥 프로그램 펑크나라 
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK ! ! !");
}
