#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

using namespace std;

//! C++11 이전 TLS 를 사용하는 방법
__deallocate(thread) int32 ord_LThread;

//! C++11 이후 TLS 를 사용하는 방법
thread_local int32 LThread = 0;

void ThreadMain(int32 threadid)
{
	LThread = threadid; //! TLS 에 현재 스레드 ID 를 저장한다.
	while (true) 
	{
		cout << "Thread ID : " << LThread << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;

	for(int32 i = 0; i < 10; ++i)
	{
		int32 threadid = i;												//~ 스레드 아이디 설정
		threads.push_back(thread(ThreadMain, threadid));	//~ 벡터 내부에 새로운 스레드를 생성한다. 인자로는 스레드 아이디어를 직접 부여
	}

	for (thread& t : threads)
	{
		t.join();	//~ 생성된 스레드들을 모두 종료시킨다.
	}
}