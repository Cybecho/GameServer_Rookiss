#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"

//! �ƹ��� Read���� �ƴҶ� Write ����!
void Lock::WriteLock()
{
	//? �߿��Ѱ�, ReadLock�� �ϳ��� ������ WriteLock�� ���� �� ����
	// ? ��, ���� ���ؼ� �ƹ��� ���� �ʰ� �־����!
	// Write�� �ƹ��� ���� �� �����ϰ� ���� ������, �����ؼ� �������� ��´�
	
	//~ ����, ������ �����尡 �̹� WRITE LOCK�� �����ϰ� �ִٸ�?
	//~ ���� WRITE COUNT MASK ������ �ִ� ��Ʈ�� ���� 16��Ʈ �κ����� �ű��
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_COUNT_MASK) >> 16;
	if (LThreadId == lockThreadId) //~ �� ID�� ���� WriteLock�� �������� Lock�� ������ �������?
	{
		_writeCount++;
		return;
	}

	//~ [WWWWWWWW] [WWWWWWWW]  ������ �� lock��ȣ�� �����ϸ� ������ ��
	//~ �׷�, �� ������ ���̵��� LTreadId�� [W]������ 16��Ʈ�� ��� �Ű���
	const int64 beginTick = GetTickCount64();										// ���� �ð�
	const uint32 desired = ((LThreadId << 16) & WRITE_COUNT_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{	//? 5000������ ���ɶ� ��浹�� WRITE LOCK ����~
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{	//? ������� ��������, ���� WRITE LOCK�� ��������
				_writeCount++;
				return;
			}
		}
		// ��� �ð��� �ʹ� �����ɸ��ٸ� �׳� ���α׷� ũ���ó���! 
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK TIMEOUT ! ! !");

		this_thread::yield();	//? 5000�� ���ɶ� ���Ҵµ��� ���������, �ٸ� �����忡�� �纸�ϰ� �ٽ� �õ��غ���~
	}
}

void Lock::WriteUnlock()
{
	//~ ��! ��ó�� Write�� �ƹ��� Read�� �ϰ����������� �����ϴϱ�, 
	//~ ���� Write���� �ʴ´ٴ°���,
	//~ �ᱹ Write��Ʈ�� Read��Ʈ �� �� 0�� �Ǵ� ��Ȳ�̰ڱ���!

	if((_lockFlag.load() & READ_COUNT_MASK) != 0) // �д� �����尡 �ϳ��� ������ WriteLock�� ������ �� ����
		CRASH("READ LOCK IS ALREADY EXIST ! ! !");

	const int32 lockCount = --_writeCount; // ���� ������ WriteLock�� �ϳ� ���δ�
	if (lockCount == 0)								// ���� ������ WriteLock�� ���ٸ�
		_lockFlag.store(EMPTY_FLAG);			// ���� ������ lockFlag�� Empty�� ������ش�
		
}

//! �ƹ��� Write���� �ƴҶ� Read ����!
void Lock::ReadLock()
{
	//~ �̹� WRITE LOCK�� �����ϰ� �ִ� ���¿��� READ�� �Ϸ����Ѵٸ�?
	//~ ���� WRITE COUNT MASK ������ �ִ� ��Ʈ�� ���� 16��Ʈ �κ����� �ű��
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_COUNT_MASK) >> 16;
	if (LThreadId == lockThreadId) //~ �� ID�� ���� WriteLock�� �������� Lock�� ������ �������?
	{
		_lockFlag.fetch_add(1); // ���� �̹� write�� ������������, �ٸ� ������� ��ó�� �������ݾ�?
		return;
	}

	// �ƹ���  ReadLock�� �����ϰ� ���� ������, �����ؼ� �������� ��´�
	const int64 beginTick = GetTickCount64();										// ���� �ð�
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//~ READ 16��Ʈ ������ �����ϰ�, WRITE 16��Ʈ ������ ��������� ReadLock�� ��ƾ���
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected+1))
				return; //~ �� ��Ȳ�� ������, expected���� 1�������Ѽ� READ LOCK�� ��������
		}
		// ��� �ð��� �ʹ� �����ɸ��ٸ� �׳� ���α׷� ũ���ó���! 
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK TIMEOUT ! ! !");

		this_thread::yield(); // 5000�� ���ɶ� ���Ҵµ��� ���������, �ٸ� �����忡�� �纸�ϰ� �ٽ� �õ��غ���~
	}
}

void Lock::ReadUnlock()
{
	//~ fetch_sub �� ���������ڶ� �Ȱ��Ƽ� ������ ��µ�, �������� �̹� 0�̾�? 
	//~ ���� ��Ȳ, �׳� ���α׷� ��ũ���� 
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK ! ! !");
}
