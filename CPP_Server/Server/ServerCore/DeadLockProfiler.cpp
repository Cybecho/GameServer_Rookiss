#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);	// 멀티 스레드 환경이니 일단 락을 잡아줌
	
	int32 lockId = 0;														// 아이디를 찾거나 발급한다
	auto findit = _nameToId.find(name);							// 이름으로도 찾아보고 (만약 이름이 있으면, 이미 방문한 정점이란 거겠죠?)
	if (findit == _nameToId.end())									//! 만약 찾는 이름이 없었다면, 처음 방문한 정점일태니, Id를 새로 발급해줄거다
	{
		lockId = static_cast<int32>(_nameToId.size());		// 새로운 lockId는 _nameToId의 사이즈로 할당 (0,1,2 ... 순서로 할당)
		_nameToId[name] = lockId;									// 새로 받은 이름과 lockId를 맵핑해준다
		_idToName[lockId] = name;									// 새로 받은 lockId와 이름을 맵핑해준다
	}
	else																		//! 만약 찾는 이름이 있었다면, 이미 방문한 정점이니, Id를 찾아서 할당해줄거다
	{
		lockId = findit->second;										// 이미 있는 이름이니까, 그 이름에 해당하는 lockId를 찾아서 할당해준다
	}

	//~ 잡고 있는 락이 있었다면
	if (_lockStack.empty() == false)
	{																	// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인
		const int32 prevId = _lockStack.top();			// 이전에 잡은 락의 아이디를 가져온다 (stack의 최상단)
		if (lockId != prevId)												//! 이전에 잡은 락과 현재 잡은 락이 다르다면 (재귀적으로 Lock을 잡을 수 있는 상태이기에, 같은 Lock을 한번 더 취득하는 상황은 데드락 상황이 아니기에 그냥 넘어가겠지?)
		{																		// 해당 if문에 들어온거면, 결국 다른 락을 잡은거기 떄문에, 사이클을 체크해줘야한다
			set<int32>& history = _lockHistory[prevId];			// 이전에 잡은 락의 히스토리를 가져온다
			if (history.find(lockId) == history.end())						//! 새로운 간선을 발견한 경우 (신규 락을 획득한 경우)
			{
				history.insert(lockId);											// 락의 히스토리에 새로 잡은 락을 넣어준다
				CheckCycle();														// 사이클을 체크해준다 (신규 락을 획득했을때 사이클 체크!)
			}
		}
	}
	_lockStack.push(lockId);										// 락을 스택에 푸시해준다
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);	// 멀티 스레드 환경이니 일단 락을 잡아줌

	//~ 락 획득시 오류 예외 처리
	if (_lockStack.empty())						//! 근데 애초에 락을 잡은게 없다면, 뭔가 잘못된거임
		CRASH("MULTIPLE_UNLOCK");	
	
	int32 lockId = _nameToId[name];		// 이름으로부터 lockId를 찾아온다
	if (_lockStack.top() != lockId)			//! 만약 현재 잡아둔 락스택 최상단의 락과 찾은 lockId가 다르다면, 뭔가 잘못된거임
		CRASH("INVALID_UNLOCK");

	_lockStack.pop();			// 락을 스택에서 팝해준다
}

void DeadLockProfiler::CheckCycle()
{
	//~ 변수 초기화
	const int32 lockCount = static_cast<int32>(_nameToId.size());	// 락의 개수를 가져온다
	_discoveredOrder = vector<int32>(lockCount, -1);					// 노드가 발견된 순서를 기록하는 lockCount번째 배열을 -1로 초기화한다
	_discoveredCount = 0;															// 노드가 발견된 카운터 횟수를 0으로 초기화한다
	_finished = vector<bool>(lockCount, false);							// Dfs(i)가 종료되었는지 확인하는 lockCount위치의 배열을 false로 초기화한다
	_parent = vector<int32>(lockCount, -1);									// Dfs(i)에서 i의 부모를 저장하는 lockCount위치의 배열을 -1 로 초기화한다 (내가 어떤 부모로부터 발견되었는지 저장하는 변수)

	//~ DFS 탐색
	for (int32 lockId = 0; lockId < lockCount; lockId++)					// 현재 존재하는 모든 락에 대해서 DFS를 돌려준다
		Dfs(lockId);

	//~ 연산이 끝났으면 Vector 내부에 데이터들을 정리해준다
	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();

}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)						//! 이미 현재 지점이 발견되었었다면
		return;														// 그냥 리턴해준다 (-1은 한번도 방문하지 않은 노드를 의미함)

	_discoveredOrder[here] = _discoveredCount++;	// 현재 노드의 발견 순서를 기록해준다
	
	//~ 모든 인접한 정점을 순회한다
	auto findit = _lockHistory.find(here);					// 현재 노드의 히스토리를 찾아온다
	if (findit == _lockHistory.end())							// 히스토리를 판별했는데도, 히스토리가 없었다면
	{																	// 이 정점의 락을 잡은 상태에서 다른 락을 잡은적이 없다는 뜻이니까
		_finished[here] = true;									// 현재 노드의 방문을 완료했다고 표시해준다
		return;														// 그리고 리턴해준다
	}
	//~ 여길 넘어왔으면, 내가 락을 잡고, 또 다른 락을 잡은적이 있다는 뜻이잖아?
																		// 그럼 이제 현재 그래프에 사이클이 있는지 확인해봐야지?
	set<int32>& nextSet = findit->second;				// secondf? findit = _lockHistory; 이니까, map<int32, set<int32>>  이여서 set<int32>를 가져온다
	for (int32 there : nextSet)
	{																	// 아직 방문한 적이 없다면 방문할것이다
		if (_discoveredOrder[there] == -1)				//! 만약 방문한적이 없다면, 방문해줄거다
		{
			_parent[there] = here;								// "너(here)는 나(there) 때문에 방문된거다! 하고 도장 쾅 찍는거"
			Dfs(there);												// 재귀적으로 다음 노드를 방문해준다
			continue;
		}

		//~ 이미 방문한적이 있다면, 순방향 & 역방향 체크해볼거임
		//! here가 there보다 먼저 발견되었다면, 순방향 간선이다 (there는 here의 후손이다)
		if (_discoveredOrder[here] < _discoveredOrder[there])				// 순방향 간선이라면, 사이클이 없다
			continue;

		//! here가 there보다 늦게 발견되었다면, 역방향 간선이다 (there는 here의 선조다)
		// 순방향이 아니고, Dfs(there)가 끝나지 않았다면, there는 here의 선조라는 의미니까 사이클이 존재함!
		if (_finished[there] == false)																		// 역방향 간선이라면, 사이클이 존재한다
		{
																													//~ 여기서부턴 무조건 사이클이 존재하는 영역임
			printf("%s -> %s\n", _idToName[here], _idToName[there]);					// 어디에서 데드락이 발생했는지 현재와 다음 노드를 출력해준다

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[there]);// 어디에서 데드락이 발생했는지 현재와 다음 노드를 출력해준다
				now = _parent[now];																		// 현재 노드를 부모로 바꿔준다 (부모를 계속 타고타고 올라갈거임)
				if (now == there)																				// now가 there까지 도달했으면
					break;																						// 루프를 빠져나온다
			}
			CRASH("DEADLOCK");																		// 데드락이 발생했다고 크래시를 날려준다 (위에 에러 로그들과 함께)
		}
	}

	_finished[here] = true;	// 현재 노드의 방문을 완료했다고 표시해준다
}
