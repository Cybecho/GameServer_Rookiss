#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"

void AccountManager::ProcessLogin()
{
	//! AccountManager의 락을 걸어줌
	lock_guard<mutex> guard(_mutex);	//~ 락가드 생성자에서 락을 걸어줌

	//! UserManager의 락을 걸어줌
	User* user = UserManager::Instance()->getUser(100);
}