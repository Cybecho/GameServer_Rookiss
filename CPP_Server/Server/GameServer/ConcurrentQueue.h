#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue() {};
	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex); //~ �ٸ� ��������� �ش� ť�� ���� ���ϰ� ���Ƶα�
		_queue.push(move(value));			//~ ���ʿ��� ���� ���� ���� move�� �� �־��ְ�
		_condVar.notify_one();					//~ ������� ������ �ϳ� �ƿ��!
	}

	//! Pop���� �Ǿ����� �ȵǾ����� Ȯ���ϱ� ���� bool�� ��ȯ�Ѵ�
	//! 100% Ȯ���� Pop�� �ȵǴϱ�, TryPop�̶�� �̸��� ���δ�
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty()) return false;

		value = move(_queue.front());	//! ���� ��ܿ� ���� ��������
		_queue.pop();							//! ���ÿ��� ��ܰ��� �����Ѵ�	
		return true;
	}

	bool WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); //~ notify_one�� �����ϱ� ���� unique_lock�� �����
		_condVar.wait(lock, [this] { return _queue.empty() == false; }); //~ ���ٽ��� ����ؼ� ���ÿ� ������ ������ ���������� ����� (�ٵ� ���� this ���� ����� �ɵ�)
		value = move(_queue.front());
		_queue.pop();
		return true;
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};

class ConcurrentQueue
{
};

