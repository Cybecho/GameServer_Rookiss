#pragma once
#include "Types.h"

/* --------------------------------------------------------
32비트가 쓰일거임
[WWWWWWWW] [WWWWWWWW] [RRRRRRRR ] [RRRRRRRR]
상위 16비트 : 쓰기 (이 락을 현재 획득한 쓰레드의 아이디를 기입할것임)
하위 16비트 : 읽기 (공유중인 이 락의 READ카운트를 여기 넣어둘것임)
-------------------------------------------------------- */

class Lock 
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,			// 최대 대기시간 10초
		MAX_SPIN_COUNT =			5000,			// 최대 5000번 스핀 가능
		WRITE_COUNT_MASK =		0x0000FFFF, // 상위 16비트만 뽑아오기 위한 코드
		READ_COUNT_MASK =		0xFFFF0000, // 하위 16비트만 뽑아오기 위한 코드
		EMPTY_FLAG =					0x00000000, // 락이 비어있는 상태
	};
public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;	// wirte를 이미 소유중인 스레드가 또 write할 수 있으니까 (나만 쓸거니까)

};

/* --------------------------------------------------------
커스텀 Lock Guard 제작
-------------------------------------------------------- */

// RAII 패턴 (자원 획득시 초기화, 자원 소멸시 해제)
// 자동으로 Lock 초기화, 해제를 해주면 Read & Write가 꼬일일은 적겠지?
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }	//~ 생성자에서 락 획득
	~ReadLockGuard() { _lock.ReadUnlock();  }								//~ 소멸자에서 락 해제
private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }	//~ 생성자에서 락 획득
	~WriteLockGuard() { _lock.WriteUnlock(); }								//~ 소멸자에서 락 해제
private:
	Lock& _lock;
};