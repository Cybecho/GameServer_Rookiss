#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

atomic<bool> flag;

int main()
{
	flag = false;
	flag.store(true, memory_order::memory_order_seq_cst); // flag = true
	bool val = flag.load(memory_order::memory_order_seq_cst); //  val = true

	//! 상황을 가정해보자
	//~ 1) 이전 flag값을 prev에 넣고, flag를 false로 바꾸고 싶다.
	//~ 1-1) 시도 1
	{
		flag.store(true); // flag = true

		bool prev = flag;
		flag.store(false);
		/*
		근데 만약에 flag값 prev에 옮기는 과정에서, 다른 스레드가 flag값을 가져가 바꾸면 어칼거임
		prev는 true를 기대하고 가져갔겠지만, 다른 코어의 캐시에 flag는 이미 false로 바뀌어있을 수 있음
		사실상 이미 false값으로 수정되었는데, 또 fallse로 바꾸려고하는거겠지?
		어처피 지금은 true 아님 false 둘 중 하나라 상관없지만, 이게 정수나 실수라면,, 당연히 문제가 발생하겠지
		데이터 가시성 문제가 존재할 수 밖에 없겠지?
		*/
		//? 즉, 현재 문제점은 원자적으로 한큐에 처리되지 않아 문제가 발생한다는것이다..
	}
	//~ 1-2) 시도 2
	{
		flag.store(true); // flag = true

		bool prev = flag.exchange(false); // 값을 읽고 쓰는걸 한 사이클로 처리하는 exchange 함수
	}
	//~ 1-3) 시도 3 CAS 조건부 수정
	{
		bool expected = false;		// 초기 기대값
		bool desired = true;			// 바꿀 값
		flag.compare_exchange_strong(expected, desired, memory_order::memory_order_seq_cst);
	}

	//~ Spurious Failue (가짜 실패 문제)
	{
		bool expected = false;		// 초기 기대값
		bool desired = true;			// 바꿀 값

		// compare_exchange_strong
		//compare_exchange_strong은 코드를 무조건 완수함
		if (flag == expected)
		{
			flag = desired;
			return true;
		}
		else
		{
			expected = flag;
			return false;
		}

		// compare_exchange_weak
		// compare_exchange_weak은 실패할 수 있음
		// 원래는 무조건 성공해야하는건데, 하드웨어적 예외로 코드가 실패할 수 있음
		if (flag == expected)
		{
			//~ 하드웨어적인 이유로 중간에 실패할 수 있음
			// if(특정 예외 상황)
			flag = desired;
			return true;
		}
		else
		{
			expected = flag;
			return false;
		}
	}
}