#pragma once

#include <thread>
#include <functional>



class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void			Lauch(function<void(void)> callback);
	void			Join();
	static void	InitTLS();			// TLS 초기화
	static void	DestroyTLS();		// TLS 해제
private:
	Mutex				_lock;
	vector<thread>	_threads;
};