/*
 * Created by Yeonhyuk on 2024-09-08.
 * 수동 리셋 모드, 주기 타이머
 *
 * 결국 X초 이후 X초에 한번씩 울리는 타이머를 만들 수 있음
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
        false, // 수동 리셋 모드
        _T("WaitableTimer")
        );
    if (hTimer == nullptr) {
        _tprintf(_T("CreateWaitableTimer failed (%d) \n"), GetLastError());
        return -1;
    }

    _tprintf(_T("Waiting for 5 seconds... \n"));

    SetWaitableTimer(hTimer, &liDueTime, 1000, nullptr, nullptr, false);

    while (1) {
        WaitForSingleObject(hTimer, INFINITE);
        _tprintf(_T("Timer was signaled. \n"));
        MessageBeep(MB_ICONEXCLAMATION);

        // 타이머를 수동으로 해제할 수도 있음
        // 하지만 타이머를 해제한다는게 리소스를 해제한다는 뜻은 아님
        // 때문에 프로그램 종료 시 CloseHandle 을 해주어야 함
        // CancelWaitableTimer(hTimer);
    }

    CloseHandle(hTimer);

    return 0;
}