#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>

atomic<int32> sum =	0;

void Add() 
{
	for (int32 i = 0; i < 1'000'000; i++)
	{
		//sum ++;				// 그냥 sum++ 하면 이게 int인지 atomic<int>인지 몰라서 애매하다.
		sum.fetch_add(1);	// 그래서 atomic을 명시하기 위해 이렇게 해야한다.
	}
}

void Sub()
{
	for (int32 i = 0; i < 1'000'000; i++)
	{
		//sum--;				// 그냥 sum-- 하면 이게 int인지 atomic<int>인지 몰라서 애매하다.
		sum.fetch_sub(1);	// 그래서 atomic을 명시하기 위해 이렇게 해야한다.
	}
}

int main()
{
	thread t1(Add);
	thread t2(Sub);

	if(t1.joinable()) t1.join();
	if(t2.joinable()) t2.join();

	cout << sum << endl;
}