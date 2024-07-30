﻿#include "pch.h"
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

class EventLock
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

			// 방법 1 : 실패하면 100ms 뒤에 CPU 점유권 포기
			this_thread::sleep_for(std::chrono::microseconds(100));
			// 방법 2 : 실패하면 0초 뒤에 바로 CPU점유권 포기
			this_thread::sleep_for(0ms);
			// 방법 3 : yield() 를 통해 현재 CPU점유권 양보하기
			this_thread::yield();
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
EventLock eventLock;

void Add() 
{
	for (int32 i = 0; i < 100'000; i++) 
	{
		lock_guard<EventLock> guard(eventLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < 100'000; i++)
	{
		lock_guard<EventLock> guard(eventLock);
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