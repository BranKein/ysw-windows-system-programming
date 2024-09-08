/*
 * Created by Yeonhyuk on 2024-09-08.
 * 수동 리셋 타이머를 사용한 스레드 동기화
 *
 * 정해진 시간이 지나면 자동으로 Signaled 상태로 변경
 * 이벤트 오브젝트와 비슷하게 수동 리셋 모드와 자동 리셋 모드가 있음
 * 이벤트 오브젝트와 동일한 리셋 개념을 가짐
 *
 * 또한 주기를 설정할 수 있음
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
        _tprintf(_T("CreateWaitableTimer failed (%d) \n"), GetLastError());
        return -1;
    }

    _tprintf(_T("Waiting for 5 seconds... \n"));

    SetWaitableTimer(hTimer, &liDueTime, 0, nullptr, nullptr, false);

    WaitForSingleObject(hTimer, INFINITE);
    _tprintf(_T("Timer was signaled. \n"));
    MessageBeep(MB_ICONEXCLAMATION);

    return 0;
}