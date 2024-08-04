#include "pch.h"
#include "CorePch.h" 
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

using namespace std;

LockQueue<int32> q;
LockStack<int32> s;

void Push()
{
	while (true)
	{
		{
			int32 value = rand() % 10;
			q.Push(value);
			this_thread::sleep_for(1s);
		}
	}
}

void Pop()
{
	while (true)
	{
		int32 data = 0;
		if (q.TryPop(OUT data))						// 버전 1
		if (q.WaitPop(OUT data))						// 버전 2
			cout << "Pop : " << data << endl;

		this_thread::sleep_for(1s);
	}
}

int main()
{
	thread t1(Pop);
	thread t2(Push);
	t1.join();
	t2.join();
}