#pragma once
class PlayerManager
{
	USE_LOCK;
public:
	void PlayerThenAccount();			// 플레이어락을 먼저 잡고 어카운트락을 잡을거라는 의미
	void Lock();								// 그냥 락을 잡는 함수
};

extern PlayerManager GPlayerManager;