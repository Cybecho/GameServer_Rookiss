#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <future>

int32 buffer[10'000][10'000];

int main()
{
	memset(buffer, 0, sizeof(buffer)); // 배열을 전부 0값으로 초기화

	//! [i][j] 순으로 더하기
	{
		uint64 start = GetTickCount64(); //~ 시작 시간 측정

		int64 sum = 0;
		for (int32 i = 0; i < 10'000; i++)
			for (int32 j = 0; j < 10'000; j++)
				sum += buffer[i][j];

		uint64 end = GetTickCount64(); // ~ 종료 시간 측정 
		cout << "Elapsed Time [i][j] : " << (end - start) << endl;
	}
	
	//! [j][i] 순으로 더하기
	{
		uint64 start = GetTickCount64(); //~ 시작 시간 측정

		int64 sum = 0;
		for (int32 i = 0; i < 10'000; i++)
			for (int32 j = 0; j < 10'000; j++)
				sum += buffer[j][i];

		uint64 end = GetTickCount64(); // ~ 종료 시간 측정 
		cout << "Elapsed Time [j][i] : " << (end - start) << endl;
	}
}