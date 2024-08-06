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

//! lock-free ����� ����
template<typename T>
class LockFreeStack
{
	struct Node
	{
		//~ struct �� �����ڰ� �����ϴ�
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
		++_popCount; //~ Pop�� �õ��ϴ� ������ ī��Ʈ�� ������Ų��

		Node* prevHead = _head.load();

		while (prevHead && _head.compare_exchange_weak(prevHead, prevHead->next) == false)
		{
			//~ ������ �и�
		}

		if (prevHead == nullptr)
		{
			--_popCount; //~ Pop�� �õ��ϴ� ������ ī��Ʈ�� ���ҽ�Ų��
			return false;
		}
		
		value = prevHead->data; //~ ������� ������, ��� ��ü�� �������� ������ �ִ� ��Ȳ
		TryDelete(prevHead);		//~ �����ǵ� ������ �ְڴ�, �׷� ���� ������ �õ��غ���?
		
		return true;
	}

	void TryDelete(Node* prevHead)
	{
		// �� �ܿ� ���� �ִ��� üũ�ؾ߰���? 
		if (_popCount == 1) //~ 1�� ���, ���� ����~!
		{
			// ��� ȥ���ΰ�, ���� ����� �ٸ� ���鵵 ����������
			// �ٵ�, �������Ʈ�� �ִ� �����͵� ������ ���������� �ѹ� �� Ȯ��������Ѵ�
			// �������Ʈ�� �ִ� �����͸� exchange�� ���� �����´�
			// ���� ��� ����Ʈ�� ���� ����Ʈ ����̴�, ��ǻ� ����Ʈ ��ü�� �������°Ű���?
			Node* node = _pendingList.exchange(nullptr); //~ ���� �ִ� ���� nullptr�� �о������, ������ ���� Node�� �ִ´�
			
			if(--_popCount == 0) //~ ������ 1�̾��ٰ�, �� 1�� �ٿ���0�̸�, ������ ���� �ִ°���?
			{
				// �� ���̿� ���� ���͵�, �̹� ��� ����Ʈ�� ������ ���ÿ� �и��� ��Ȳ��
				DeleteNodes(node); // �������Ʈ�� �ִ� ������ ��� �����Ѵ�
			}
			else if (node)				//~�� ���̿� ���� ��������!
			{
				// ��� ����Ʈ�� �ٽ� ���� �ڸ��� �������´�
				// ���� ���ุ �صδ� ��Ȳ��
				ChainPendingNodeList(node);
			}

			delete prevHead; // ����, ���� ������ �ִ� ��带 �����Ѵ�
		}
		else //~ 1�� �ƴ� ���, ���� �� �ֳ�?
		{
			ChainPendingNode(prevHead); //�׷�, �������Ʈ�� �־����
			--_popCount; // �׸���, ���� ������
		}
	}

	//! ü�� ��� ����Ʈ�� �����ϱ� ���ؼ� ������ ����ؾ��Ѵ�
	
	//! ü�� ��� ��� ����Ʈ �����ε� 1
	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{
		}
	}

	//! ü�� ��� ��� ����Ʈ �����ε� 2
	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)		// ��� ������ �̵�
			last = last->next;

		ChainPendingNodeList(node, last);
	}

	//! �ϳ��� �����Ҷ�
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

	atomic<int32> _popCount = 0; // Pop�� �õ����� �����带 ī��Ʈ�ϱ� ���� ����
	atomic<Node*> _pendingList;	// ���� �Ǿ�� �� ����
};