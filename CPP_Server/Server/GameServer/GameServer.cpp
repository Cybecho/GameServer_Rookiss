#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>

mutex m;
queue<int32> q;

condition_variable cv; // CV는 커널 오브젝트가 아니다 (유저 오브젝트임)

//! 데이터를 삽입하는 스레드 (생산자)
void Producer()
{
	while (true)
	{
		// 1) Lock을 걸고
		// 2) 공유변수 데이터를 수정하고
		// 3) Lock을 풀고
		// 4) 조건 변수를 통해 다른 스레드가 Lock을 걸 수 있도록 해준다.

		{
			unique_lock<mutex> guard(m);		// 공유 자원에 데이터 삽입을 위해 잠금
			q.push(1000);								// 큐에 데이터 삽입
		}
		this_thread::sleep_for(1s);		// 1초 대기
		cv.notify_one(); // 대기중인 스레드중 하나의 스레드만 꺠운다.
	}
}

//! 데이터를 꺼내쓰는 스레드 (소비자)
void Consumer()
{
	while (true)
	{
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; });		// 큐가 비어있지 않다면 깨운다.
		// 1) Lock을 걸고
		// 2) 조건 변수 확인
		// -> 만족 O : 대기wait 을 빠져나와 아래 코드를 실행		(큐에 데이터가 있다는 소리)
		// -> 만족 X : 대기wait 상태로 전환							(큐에 데이터가 없다는 소리)

		// notify_one 할때 lock을 잡고 있는것이 아니기 때문에 wait에서 lock을 넘겨줘야한다.
		{
			int32 data = q.front();
			q.pop();
			cout << "Queue Size: " << q.size() << " Data: " << data << endl;
		}
	}
}

int main()
{

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();
}