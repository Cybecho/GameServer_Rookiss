#pragma once

#include <mutex>

//! lock 기반의 스택
template<typename T>
class LockStack
{
public:
	LockStack() {};
	~LockStack() {};

	LockStack(const LockStack&) = delete; //! 복사 생성자는 삭제해버린다
	LockStack& operator=(const LockStack&) = delete; //! 대입 연산자도 삭제해버린다 (복사하려고해도, 응 돌아가~)

	//! Push할때 공유 데이터에 접근하니 lock을 걸어준다
	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(move(value));				//~ move를 사용해서 불필요한 복사를 방지한다
		_condVar.notify_one();						//~데이터가 들어왔으니, 대기중인 스레드를 깨워준다
	}

	//! Pop값은 되었는지 안되었는지 확인하기 위해 bool을 반환한다
	//! 100% 확률로 Pop이 안되니까, TryPop이라고 이름을 붙인다
	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty()) return false;

		value = move(_stack.top()); //! 스택 상단에 값을 가져오고
		_stack.pop();						//! 스택에서 상단값을 제거한다	
		return true;
	}

	bool WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex); //~ notify_one을 인지하기 위해 unique_lock을 사용함
		_condVar.wait(lock, [this] { return _stack.empty() == false; }); //~ 람다식을 사용해서 스택에 데이터 없으면 있을때까지 대기함 (근데 여기 this 굳이 안적어도 될듯)
		value = move(_stack.top());
		_stack.pop();
		return true;
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};