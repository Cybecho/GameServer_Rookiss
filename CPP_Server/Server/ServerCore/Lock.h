#pragma once
#include "Types.h"

/* --------------------------------------------------------
32��Ʈ�� ���ϰ���
[WWWWWWWW] [WWWWWWWW] [RRRRRRRR ] [RRRRRRRR]
���� 16��Ʈ : ���� (�� ���� ���� ȹ���� �������� ���̵� �����Ұ���)
���� 16��Ʈ : �б� (�������� �� ���� READī��Ʈ�� ���� �־�Ѱ���)
-------------------------------------------------------- */

class Lock 
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,			// �ִ� ���ð� 10��
		MAX_SPIN_COUNT =			5000,			// �ִ� 5000�� ���� ����
		WRITE_COUNT_MASK =		0x0000FFFF, // ���� 16��Ʈ�� �̾ƿ��� ���� �ڵ�
		READ_COUNT_MASK =		0xFFFF0000, // ���� 16��Ʈ�� �̾ƿ��� ���� �ڵ�
		EMPTY_FLAG =					0x00000000, // ���� ����ִ� ����
	};
public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;	// wirte�� �̹� �������� �����尡 �� write�� �� �����ϱ� (���� ���Ŵϱ�)

};

/* --------------------------------------------------------
Ŀ���� Lock Guard ����
-------------------------------------------------------- */

// RAII ���� (�ڿ� ȹ��� �ʱ�ȭ, �ڿ� �Ҹ�� ����)
// �ڵ����� Lock �ʱ�ȭ, ������ ���ָ� Read & Write�� �������� ������?
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) { _lock.ReadLock(); }	//~ �����ڿ��� �� ȹ��
	~ReadLockGuard() { _lock.ReadUnlock();  }								//~ �Ҹ��ڿ��� �� ����
private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }	//~ �����ڿ��� �� ȹ��
	~WriteLockGuard() { _lock.WriteUnlock(); }								//~ �Ҹ��ڿ��� �� ����
private:
	Lock& _lock;
};