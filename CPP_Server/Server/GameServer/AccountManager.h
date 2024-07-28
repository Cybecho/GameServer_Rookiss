#pragma once
#include <mutex>

class Account 
{
	// TODO
	// ����� ������ �ܶ� ������ 
};

//! ���ӿ� �������� ����ڵ��� ������ ������ �� �ִ� Ŭ����	
class AccountManager
{
public:
	static AccountManager* Instance()
	{
		static AccountManager instance;
		return &instance;
	}

	//! ����� id�� ���� ����� ������ �������� �Լ�
	Account* GetAccount(int32 id) 
	{
		lock_guard<mutex> guard(_mutex);	//~ ������ �����ڿ��� ���� �ɾ���
		return nullptr;
	};

	void ProcessLogin(); //~ �ش� �κп��� �α��� ó���� �� ���̹Ƿ� ���� �ɾ���

private:
	mutex _mutex;	//~ ��Ƽ������ ȯ�濡�� ����� ������ ���� ������ �����ϱ� ���� ���ؽ�
	//map<int32, Account*> _accounts;	//~ ����� ������ ���� ��
};

