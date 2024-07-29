#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

class SpinLock
{
public:
	void lock()
	{
		// CAS (Compare And Swap)
		bool expected = false; // 현재 상태가 free 이다. (기대값)
		bool desired = true;   // 현재 상태가 lock 이다.

		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false; // 해당 함수는 &bool 로 받아오기 때문에, 다시 false 로 초기화 해줘야 한다.
		}

	};

	void unlock()
	{
		_locked.store(false); // store() 함수는 std::atomic 클래스의 멤버 함수로, 원자적으로 값을 저장하는 역할을 합니다. 
		// 이 코드에서 _locked.store(false);는 _locked 변수에 false 값을 원자적으로 저장합니다.
	};
private:
	atomic<bool> _locked = false;
};

int32 sum = 0;
mutex m;
SpinLock spinLock;

void Add() 
{
	for (int32 i = 0; i < 100'000; i++) 
	{
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < 100'000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}


int main() 
{

	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}