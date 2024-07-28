#include "pch.h"
#include "CorePch.h"
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>

vector<int32> v;	//~ 벡터 선언
mutex key;			//~ 뮤텍스 선언 (상호 배타적)

//! RAII 설정
template<typename T> //~ 뭐가 들어올지 모르니 일단은 템플릿으로
class LockGuard
{
public:
	//! 생성자
	LockGuard(T& m) //~ 템플릿 레퍼런스를 인자로 받음
	{
		_mutex = &m;	// 
		_mutex->lock();	// 생성자에서 lock을 대신 호출해줄거임
	}
	~LockGuard()
	{
		_mutex->unlock(); //~ 소멸자에서 unlock을 대신 호출해줄거임
	}
private:
	T* _mutex; //~ 뮤텍스를 가리킬 포인터
};

void Push()
{
	for (int i = 0; i < 10'000; ++i)
	{
		LockGuard<mutex> lockGuard(key);//~ 락가드 생성자에서 락을 걸어줌
		v.push_back(i);					//~ 해당 쓰레드 벡터 데이터 점유
		if(i==5000) { break; };			//~ 5000번째에서 락 해제
	}
}

int main()
{
	v.reserve(20'000);

	thread t1(Push);
	thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;

	return 0;
}