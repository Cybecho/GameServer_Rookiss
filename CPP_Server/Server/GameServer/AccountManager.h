#pragma once
#include <mutex>

class Account 
{
	// TODO
	// 사용자 정보가 잔뜩 담기겠지 
};

//! 게임에 접속중인 사용자들의 정보를 관리할 수 있는 클래스	
class AccountManager
{
public:
	static AccountManager* Instance()
	{
		static AccountManager instance;
		return &instance;
	}

	//! 사용자 id를 통해 사용자 정보를 가져오는 함수
	Account* GetAccount(int32 id) 
	{
		lock_guard<mutex> guard(_mutex);	//~ 락가드 생성자에서 락을 걸어줌
		return nullptr;
	};

	void ProcessLogin(); //~ 해당 부분에서 로그인 처리를 할 것이므로 락을 걸어줌

private:
	mutex _mutex;	//~ 멀티쓰레드 환경에서 사용자 정보에 대한 접근을 제어하기 위한 뮤텍스
	//map<int32, Account*> _accounts;	//~ 사용자 정보를 담을 맵
};

