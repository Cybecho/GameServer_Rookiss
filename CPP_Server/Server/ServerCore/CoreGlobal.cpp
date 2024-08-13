#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr; // ���� ������ �Ŵ����� �ʱ�ȭ

CoreGlobal::CoreGlobal()
{
	GThreadManager = new ThreadManager(); // ���� ������ �Ŵ��� ����
}

CoreGlobal::~CoreGlobal()
{
	delete GThreadManager; // ���� ������ �Ŵ��� ����
}
