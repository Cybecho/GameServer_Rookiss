#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;

volatile bool ready; // 상황을 체크하기 위해 불라틸 변수 설정

void Thread_1()
{
	while (!ready); // ready가 true가 될 때까지 뺑뻉이 돌며 대기

	y = 1;		// Store y (y값에 R value 덮어쓰기)
	r1 = x;	// Load x (x값을 L value에 저장)
}

void Thread_2()
{
	while (!ready); // ready가 true가 될 때까지 뺑뻉이 돌며 대기

	x = 1;
	r2 = y;
}

int main()
{
	int32 count = 0;

	while (true)
	{
		ready = false; //~ ready를 false로 초기화
		count++;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true; //~ ready를 true로 초기화 (스레드 함수는 내부에서 while을 탈출하겠지?)
		t1.join();
		t2.join();
		cout << "r1: " << r1 << ", r2: " << r2 << endl;

		if (r1 == 0 && r2 == 0)
			break; // r1과 r2가 동시에 0이 되는 상황이 있다면 while을 빠져나가겠지?
	}

	cout << count << " 번 만에 탈출성공" << endl;
}