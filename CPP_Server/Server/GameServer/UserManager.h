#pragma once
#include <mutex>

class User
{
	// TODO
	// 사용자 정보가 잔뜩 담기겠지 

};

class UserManager
{
public:
	//! 싱글턴 패턴을 사용하기 위한 인스턴스 반환 함수
	static UserManager* Instance() 
	{
		static UserManager instance;
		return &instance;
	}

	User* getUser(int32 id) 
	{
		lock_guard<mutex> guard(_mutex);	//~ 락가드 생성자에서 락을 걸어줌
		// 그리고 여기서 사용자 데이터를 가져오겠지?
		return nullptr;
	};

	void ProcessSave(); //~ 해당 부분에서 저장 처리를 할 것이므로 락을 걸어줌

private:
	mutex _mutex;
};