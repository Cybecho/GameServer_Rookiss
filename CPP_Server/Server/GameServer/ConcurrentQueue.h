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
		lock_guard<mutex> lock(_mutex); //~ 다른 스레드들이 해당 큐에 접근 못하게 막아두기
		_queue.push(move(value));			//~ 불필요한 복사 막기 위해 move로 값 넣어주고
		_condVar.notify_one();					//~ 대기중인 스레드 하나 꺠우기!
	}

	//! Pop값은 되었는지 안되었는지 확인하기 위해 bool을 반환한다
	//! 100% 확률로 Pop이 안되니까, TryPop이라고 이름을 붙인다
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty()) return false;

		value = move(_queue.front());	//! 스택 상단에 값을 가져오고
		_queue.pop();							//! 스택에서 상단값을 제거한다	
		return true;
	}

	bool WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); //~ notify_one을 인지하기 위해 unique_lock을 사용함
		_condVar.wait(lock, [this] { return _queue.empty() == false; }); //~ 람다식을 사용해서 스택에 데이터 없으면 있을때까지 대기함 (근데 여기 this 굳이 안적어도 될듯)
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

