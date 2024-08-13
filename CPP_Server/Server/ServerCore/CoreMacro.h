#pragma once

// define에서 크래시 상황을 정의할 수 있다
// 컴파일단에서 CRASH나는것이 아니라 런타임에서 CRASH나는 상황을 볼것임
// define을 여러줄 걸쳐 선언하려면 \ 을 붙여줘야함 
// __analysis_assume(crash != nullptr) 부분은
// cause 부분에 컴파일러가 NULL을 넣어주는 과정에서 컴파일러가 에러를 잡아주겠지?
// 컴파일러에게는 일단 crash는 nullptr이 아니라고하고
// 우리가 crash에 직접 특정값을 넣음으로서 프로그램을 직접 뻗게한다
// 즉, 컴파일러를 속이는 과정임
#define CRASH(cause)                          \
do {                                                    \
    uint32* crash = nullptr;                      \
    __analysis_assume(crash != nullptr);   \
    *crash = 0xDEADBEEF;                     \
} while(0)


// 조건부로 CRASH를 내고싶을때
// 예를들어 특정 값이 무조건 0이여야하는데, 1이 들어가면 프로그램을 크래시 시켜버리는것!
#define ASSERT_CRASH(expr)      \
{                                               \
    if(!(expr))                               \
	{                                           \
		CRASH("ASSERT_CRASH"); \
        __analysis_assume(expr);     \
	}                                           \
}