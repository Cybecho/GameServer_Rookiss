#pragma once
#include <stack>
#include <map>
#include <vector>

/*--------------------
	DeadLockProfiler
---------------------*/

class DeadLockProfiler
{
public:	
	// 해당 코드에선 char name을 lock의 이름으로 사용하고 있음
	void  PushLock(const char* name);
	void  PopLock(const char* name);
	void CheckCycle();

private:	//~ 헬퍼 메소드:
	void Dfs(int32 index);

private:	//~ 데이터 구조 및 상태 관리
	unordered_map<const char*, int32> _nameToId;	// 키의 이름과 락 번호를 map으로 관리할것임. 
	unordered_map<int32, const char*> _idToName;	// 락 번호와 키의 이름을 map으로 관리할것임. 
	stack<int32>									_lockStack;		// 락 번호를 저장할 stack (DFS는 기본적으로 stack에서 작동)
	map<int32, set<int32>>					_lockHistory;	// 락 번호와 락 번호의 히스토리를 저장할 map (이전에 방문한 정점인지 아닌지 알아야 할거아님!)

	Mutex _lock;														// 멀티스레드 환경에서 사용할 뮤텍스
private:	//~ DFS(깊이 우선 탐색) 알고리즘 관련:
	vector<int32>		_discoveredOrder;						// 노드가 발견된 순서를 기록하는 배열
	int32					_discoveredCount;						// 노드가 발견된 횟수
	vector<bool>		_finished;									// Dfs(i)가 종료되었는지 확인 
	vector<int32>		_parent;										// Dfs(i)에서 i의 부모를 저장
};  