#include "pch.h"
#include "PlayerManager.h"
#include "AccountManager.h"

PlayerManager GPlayerManager;

void PlayerManager::PlayerThenAccount()
{
	WRITE_LOCK;

	this_thread::sleep_for(1s); // 이 사이 시간이 너무 짧아서 잘 동작하는것처럼 보이지만, 1초 딜레이를 주면 데드락 상황이 발생함

	GAccountManager.Lock();
}

void PlayerManager::Lock()
{
	WRITE_LOCK;
}