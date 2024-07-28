#include "pch.h"
#include "UserManager.h"
#include "AccountManager.h"

void UserManager::ProcessSave()
{
	//! AccountManager의 락을 걸어줌
	Account* account = AccountManager::Instance()->GetAccount(100);

	//! UserManager의 락을 걸어줌
	lock_guard<mutex> guard(_mutex);	//~ 락가드 생성자에서 락을 걸어줌
}