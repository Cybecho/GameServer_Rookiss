#include "pch.h"
#include "UserManager.h"
#include "AccountManager.h"

void UserManager::ProcessSave()
{
	//! AccountManager�� ���� �ɾ���
	Account* account = AccountManager::Instance()->GetAccount(100);

	//! UserManager�� ���� �ɾ���
	lock_guard<mutex> guard(_mutex);	//~ ������ �����ڿ��� ���� �ɾ���
}