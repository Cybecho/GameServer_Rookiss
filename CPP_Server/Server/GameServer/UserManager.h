#pragma once
#include <mutex>

class User
{
	// TODO
	// ����� ������ �ܶ� ������ 

};

class UserManager
{
public:
	//! �̱��� ������ ����ϱ� ���� �ν��Ͻ� ��ȯ �Լ�
	static UserManager* Instance() 
	{
		static UserManager instance;
		return &instance;
	}

	User* getUser(int32 id) 
	{
		lock_guard<mutex> guard(_mutex);	//~ ������ �����ڿ��� ���� �ɾ���
		// �׸��� ���⼭ ����� �����͸� ����������?
		return nullptr;
	};

	void ProcessSave(); //~ �ش� �κп��� ���� ó���� �� ���̹Ƿ� ���� �ɾ���

private:
	mutex _mutex;
};