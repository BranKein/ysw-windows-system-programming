/*
 * Created by Yeonhyuk on 2024-09-08.
 * 타이머의 시간을 유동적으로 변경할 수 있을까? 에 대한 실험
 *
 * 타이머에게 기다리는 시간을 LARGE_INTEGER 의 주소값을 넘기고 있기 때문에
 * 해당 주소값의 값을 변경하면 시간이 변경될 수도 있겠다는 생각을 하게 됨
 *
 * 물론 타이머가 시작할 때 해당 주소값에 들어있는 값을 가져와서 타이머를 만들면 그대로 시간이 유지될 것임
 *
 * 결과: 여전히 5초네요.
 *
 */

#include <cstdio>
#include <windows.h>
#include <process.h>
#include <tchar.h>

// 해당 프로그램이 WIN NT 이상에서 실행되었으니 SetWaitableTimer 함수의 호출을 허용해달라는 의미
// 말뜻은 WIN NT 이상에서만 SetWaitableTimer 함수를 사용할 수 있음
#define _WIN32_WINNT 0x0400

int _tmain() {
    HANDLE hTimer = nullptr;
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -50000000LL;

    hTimer = CreateWaitableTimer(
        nullptr,
        false,
        _T("WaitableTimer")
        );
    if (hTimer == nullptr) {
        _tprintf(_T("CreateWaitableTimer failed (%lu) \n"), GetLastError());
        return -1;
    }

    // 현재 시간 저장
    DWORD now = GetTickCount();

    _tprintf(_T("Waiting for 5 seconds... \n"));

    SetWaitableTimer(hTimer, &liDueTime, 0, nullptr, nullptr, false);

    liDueTime.QuadPart = -100000000LL;

    WaitForSingleObject(hTimer, INFINITE);

    _tprintf(_T("Timer was signaled. \n"));

    // 현재 시간과 비교
    _tprintf(_T("Time difference: %lu (s) \n"), (GetTickCount() - now) / 1000);

    MessageBeep(MB_ICONEXCLAMATION);

    return 0;
}