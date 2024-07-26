#include "pch.h"
#include "CorePch.h"
#include <iostream>

#include <thread>

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloThread2(int32 num)
{
    cout << num << endl;
}

int main()
{
    //! 스레드 생성 (컨테이너)
    vector<thread> v; // 스레드 컨테이너
    
    //! 스레드에 함수를 넣어서 생성
    for (int32 i = 0; i < 10; i++)
	{
		v.push_back(thread(HelloThread2, i)); // 스레드 생성
	}

    //! 스레드를 생성했으니, 이제 join을 해줘야 한다.
    for (int32 i = 0; i < 10; i++)
    {
        if(v[i].joinable()) // joinable은 스레드가 실행중인지 확인
			v[i].join();
    }

    thread t(HelloThread2, 10); // 스레드 생성

    if (t.joinable()) // joinable은 스레드가 실행중인지 확인
        t.join();   
    
    cout << "Hello Main" << endl;
}
