#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"

void AccountManager::ProcessLogin()
{
	//! AccountManager�� ���� �ɾ���
	lock_guard<mutex> guard(_mutex);	//~ ������ �����ڿ��� ���� �ɾ���

	//! UserManager�� ���� �ɾ���
	User* user = UserManager::Instance()->getUser(100);
}