#pragma once

#include <mutex>

//! lock ����� ����
template<typename T>
class LockStack
{
public:
	LockStack() {};
	~LockStack() {};

	LockStack(const LockStack&) = delete; //! ���� �����ڴ� �����ع�����
	LockStack& operator=(const LockStack&) = delete; //! ���� �����ڵ� �����ع����� (�����Ϸ����ص�, �� ���ư�~)

	//! Push�Ҷ� ���� �����Ϳ� �����ϴ� lock�� �ɾ��ش�
	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(move(value));				//~ move�� ����ؼ� ���ʿ��� ���縦 �����Ѵ�
		_condVar.notify_one();						//~�����Ͱ� ��������, ������� �����带 �����ش�
	}

	//! Pop���� �Ǿ����� �ȵǾ����� Ȯ���ϱ� ���� bool�� ��ȯ�Ѵ�
	//! 100% Ȯ���� Pop�� �ȵǴϱ�, TryPop�̶�� �̸��� ���δ�
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) return false;

		value = move(_stack.top()); //! ���� ��ܿ� ���� ��������
		_stack.pop();						//! ���ÿ��� ��ܰ��� �����Ѵ�	
		return true;
	}

	bool WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); //~ notify_one�� �����ϱ� ���� unique_lock�� �����
		_condVar.wait(lock, [this] { return _stack.empty() == false; }); //~ ���ٽ��� ����ؼ� ���ÿ� ������ ������ ���������� ����� (�ٵ� ���� this ���� ����� �ɵ�)
		value = move(_stack.top());
		_stack.pop();
		return true;
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};