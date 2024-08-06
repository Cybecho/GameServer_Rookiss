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

//! lock-free 기반의 스택
template<typename T>
class LockFreeStack
{
	struct Node
	{
		//~ struct 도 생성자가 가능하다
		Node(const T& value) : data(value), next(nullptr) {}
		
		T data;
		Node* next;
	};

public:

	void Push(const T& value)
	{
		Node* newNode = new Node(value);
		newNode->next = _head.load();
		
		while (_head.compare_exchange_weak(newNode->next,newNode) == false )
		{
		}
	}

	bool TryPop(T& value)
	{
		++_popCount; //~ Pop을 시도하는 스레드 카운트를 증가시킨다

		Node* prevHead = _head.load();

		while (prevHead && _head.compare_exchange_weak(prevHead, prevHead->next) == false)
		{
			//~ 데이터 분리
		}

		if (prevHead == nullptr)
		{
			--_popCount; //~ Pop을 시도하는 스레드 카운트를 감소시킨다
			return false;
		}
		
		value = prevHead->data; //~ 여기까지 왔으면, 노드 자체의 소유권은 나에게 있는 상황
		TryDelete(prevHead);		//~ 소유권도 나에게 있겠다, 그럼 이제 삭제를 시도해볼까?
		
		return true;
	}

	void TryDelete(Node* prevHead)
	{
		// 나 외에 누가 있는지 체크해야겠지? 
		if (_popCount == 1) //~ 1일 경우, 나만 있음~!
		{
			// 기왕 혼자인거, 삭제 예약된 다른 노드들도 삭제해주자
			// 근데, 펜딩리스트에 있는 데이터도 삭제가 가능한지는 한번 더 확인해줘야한다
			// 펜딩리스트에 있는 데이터를 exchange를 통해 가져온다
			// 기존 펜딩 리스트는 연결 리스트 방식이니, 사실상 리스트 전체를 가져오는거겠지?
			Node* node = _pendingList.exchange(nullptr); //~ 원래 있던 값은 nullptr로 밀어버리고, 가져온 값을 Node에 넣는다
			
			if(--_popCount == 0) //~ 기존에 1이었다가, 또 1을 줄여서0이면, 아직도 나만 있는거지?
			{
				// 이 사이에 누가 들어와도, 이미 펜딩 리스트는 스레드 로컬에 분리한 상황임
				DeleteNodes(node); // 펜딩리스트에 있는 노드들을 모두 삭제한다
			}
			else if (node)				//~그 사이에 누가 끼어들었네!
			{
				// 펜딩 리스트를 다시 원래 자리로 돌려놓는다
				// 삭제 예약만 해두는 상황임
				ChainPendingNodeList(node);
			}

			delete prevHead; // 이제, 내가 가지고 있던 노드를 삭제한다
		}
		else //~ 1이 아닌 경우, 누가 더 있네?
		{
			ChainPendingNode(prevHead); //그럼, 펜딩리스트에 넣어두자
			--_popCount; // 그리고, 나도 나가자
		}
	}

	//! 체인 펜딩 리스트를 구현하기 위해선 다음을 고려해야한다
	
	//! 체인 펜딩 노드 리스트 오버로딩 1
	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{
		}
	}

	//! 체인 펜딩 노드 리스트 오버로딩 2
	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)		// 노드 끝까지 이동
			last = last->next;

		ChainPendingNodeList(node, last);
	}

	//! 하나만 연결할때
	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}
	 
private:
	atomic<Node*> _head;

	atomic<int32> _popCount = 0; // Pop을 시도중인 스레드를 카운트하기 위한 변수
	atomic<Node*> _pendingList;	// 삭제 되어야 할 노드들
};