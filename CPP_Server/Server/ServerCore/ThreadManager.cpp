#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

//~ 생성자
ThreadManager::ThreadManager() 
{
	InitTLS();		// 메인 스레드 초기화
}

//~ 소멸자
ThreadManager::~ThreadManager() 
{
	Join();			// 모든 스레드가 종료될 때까지 대기
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);	// 여러 스레드가 접근할수도 있으니, 일단 락
	_threads.push_back(thread([=]()	// 람다 함수로 쓰레드 생성
		{
								// 새 쓰레드 생성시 해야 할 작업들.
			InitTLS();		// 1) 새로운 스레드 초기화
			callback();		// 2) 콜백 호출ㅠ
			DestroyTLS();	// 3) 새로운 스레드 해제
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if(t.joinable())	// joinable()은 해당 스레드가 종료되었는지 확인
			t.join();		// 모든 스레드가 종료될 때까지 대기
	}
	_threads.clear();	// 모든 스레드가 종료되었으니, 클리어(삭제)
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;	// 스레드 아이디 생성 static이라 클래스 내에서 공유됨
	LThreadid = SThreadId.fetch_add(1);		// 스레드가 생성될때마다 1씩 증가된 아이디를 부여
}

void ThreadManager::DestroyTLS()
{
}

