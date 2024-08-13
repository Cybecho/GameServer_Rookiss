#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

CoreGlobal Core;

void ThreadMain()
{
	while (true)
	{
		cout << "This Thread : "<< LThreadid << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Lauch(ThreadMain);
	}
	GThreadManager->Join();
};