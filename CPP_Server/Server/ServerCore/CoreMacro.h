#pragma once

// define���� ũ���� ��Ȳ�� ������ �� �ִ�
// �����ϴܿ��� CRASH���°��� �ƴ϶� ��Ÿ�ӿ��� CRASH���� ��Ȳ�� ������
// define�� ������ ���� �����Ϸ��� \ �� �ٿ������ 
// __analysis_assume(crash != nullptr) �κ���
// cause �κп� �����Ϸ��� NULL�� �־��ִ� �������� �����Ϸ��� ������ ����ְ���?
// �����Ϸ����Դ� �ϴ� crash�� nullptr�� �ƴ϶���ϰ�
// �츮�� crash�� ���� Ư������ �������μ� ���α׷��� ���� �����Ѵ�
// ��, �����Ϸ��� ���̴� ������
#define CRASH(cause)                          \
do {                                                    \
    uint32* crash = nullptr;                      \
    __analysis_assume(crash != nullptr);   \
    *crash = 0xDEADBEEF;                     \
} while(0)


// ���Ǻη� CRASH�� ���������
// ������� Ư�� ���� ������ 0�̿����ϴµ�, 1�� ���� ���α׷��� ũ���� ���ѹ����°�!
#define ASSERT_CRASH(expr)      \
{                                               \
    if(!(expr))                               \
	{                                           \
		CRASH("ASSERT_CRASH"); \
        __analysis_assume(expr);     \
	}                                           \
}