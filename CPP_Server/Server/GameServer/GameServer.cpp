#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

using namespace std;

int64 Calculater()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000'000; i++)
	{
		sum += i;
	}

	return sum;
}

void PromiseWorker(promise<string>&& promise)  // 값 자체를 옮길거기 때문에 오른 참조
{
	promise.set_value("Secret");
}

void TaskWorker(packaged_task<int64(void)>&& task) // 값 자체를 옮길거기 때문에 오른 참조
{
	task(); // 전달받은 함수인 task를 그냥 실행해버리기만 할거임
	// 따로 return 은 하지 않는다. 함수 외부에서 future를 통해 결과를 받을 수 있음
}

int main()
{
	//! future
	{
		/*
		1) deferred				: 지연에서 실행하셈 (lazy evaluation)
		2) async(기본)			: 비동기로 실행하셈 (별도의 스레드 생성)
		3) deferred | async 	: 둘 중 알아서 골라서 쓰셈
		*/
		future<int64> future = async(launch::async, Calculater);
		int64 result = future.get();
	}
	
	//!promise
	{
		// 미래에 결과물을 반환할꺼라 약속하는것임 
		// (promise는 추후에 future을 반환해야만함)
		promise<string> promise;									// 이 약속은 promise가 1차적으로 가질것이고
		future<string> future = promise.get_future();		// 이 약속은 future가 2차적으로 가질것입니다

		// 이 상황에서 새로운 스레드가 생성되었다면
		thread t(PromiseWorker, move(promise)); 			// promise를 move로 넘겨줍니다

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	//! packaged_task
	{
		// packaged_task<출력타입(입력타입)>
		// 함수타입과 1:1로 맞춰야한다
		packaged_task<int64(void)> task(Calculater);		// 다른 스레드에서 Calculater를 실행하고 결과를 반환해주쇼
		future<int64> future = task.get_future();				// future를 통해 결과를 받을 수 있음

		thread t(TaskWorker, move(task));						// task를 move로 넘겨줍니다
		
		int64 sum = future.get();
		cout << sum << endl;
	}
}