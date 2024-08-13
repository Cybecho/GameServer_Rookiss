#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

//~ ������
ThreadManager::ThreadManager() 
{
	InitTLS();		// ���� ������ �ʱ�ȭ
}

//~ �Ҹ���
ThreadManager::~ThreadManager() 
{
	Join();			// ��� �����尡 ����� ������ ���
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);	// ���� �����尡 �����Ҽ��� ������, �ϴ� ��
	_threads.push_back(thread([=]()	// ���� �Լ��� ������ ����
		{
								// �� ������ ������ �ؾ� �� �۾���.
			InitTLS();		// 1) ���ο� ������ �ʱ�ȭ
			callback();		// 2) �ݹ� ȣ���
			DestroyTLS();	// 3) ���ο� ������ ����
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if(t.joinable())	// joinable()�� �ش� �����尡 ����Ǿ����� Ȯ��
			t.join();		// ��� �����尡 ����� ������ ���
	}
	_threads.clear();	// ��� �����尡 ����Ǿ�����, Ŭ����(����)
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;	// ������ ���̵� ���� static�̶� Ŭ���� ������ ������
	LThreadid = SThreadId.fetch_add(1);		// �����尡 �����ɶ����� 1�� ������ ���̵� �ο�
}

void ThreadManager::DestroyTLS()
{
}

