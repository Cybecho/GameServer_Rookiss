#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

atomic<bool> ready;
int32 value;

void Producer()
{
	value = 10;
	ready.store(true, memory_order::memory_order_release);
	// -------- realse를 통한 일종의 절취선이 생김 --------
}

void Consumer()
{
	// -------- acquire를 통한 일종의 절취선이 생김 --------
	while (ready.load(memory_order::memory_order_acquire) == false) // 계속 기다림
		; 
	cout << value << endl;
}

int main()
{
	ready = false;
	value = 0;
	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	// Memory Model(정책)
	// 1) Sequentially-Consistent (일관성 있는) - [seq_cst]
	// 가시성 문제가 바로 해결됨
	// 코드 재배치 문제도 해결됨
	// 2) Acquire-Release (획득-방출) - [acq_rel]
	// 3) Relaxed (완화된) - [relaxed]
}