#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

//! 데이터를 삽입하는 스레드 (생산자)
void Producer()
{
	while (true)
	{
		//~ 유니크 락은 클래스로 설계되어, 스코프를 벗어나면 자동으로 소멸자가 실행되며 unlock 해준다
		{
			unique_lock<mutex> guard(m);		// 공유 자원에 데이터 삽입을 위해 잠금
			q.push(1000);								// 큐에 데이터 삽입
		}
		::SetEvent(handle);								// 데이터 삽입 후 event에 이벤트 발생
		//! 커널 오브젝트의 상태를 Signal 상태로 변경하고 대기중인 스레드를 깨운다!

		this_thread::sleep_for(1000ms);			//  0.1초 쉰 후 CPU 점유 포기 (lock free)
	}
}

//! 데이터를 꺼내쓰는 스레드 (소비자)
void Consumer()
{
	while (true)
	{
		//! handle에서 신호가 올때까지
		::WaitForSingleObject(handle, INFINITE);	// event가 발생할 때까지 대기

		//~ unique_lock 소멸자 unlock을 위한 단순 스코프 영역
		{
			unique_lock<mutex> guard(m);		// 공유 자원에 데이터 삽입을 위해 잠금
			if (q.empty() == false)					// 큐가 비어있지 않다면
			{
				int32 data = q.front();
				q.pop();
				cout << "Data: " << data << endl;
			}
		}
	}
}

int main()
{
	handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle); // 이벤트 해제
}