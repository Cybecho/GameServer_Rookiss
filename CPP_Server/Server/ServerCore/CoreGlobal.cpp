#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr; // 전역 스레드 매니저를 초기화

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager(); // 전역 스레드 매니저 생성
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager; // 전역 스레드 매니저 삭제
}
