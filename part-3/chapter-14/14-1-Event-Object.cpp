/*
 * Created by Yeonhyuk on 2024-09-08.
 * 이벤트 기반 스레드 동기화
 *
 * 스레드의 실행순서를 동기화 하기 위해 이벤트 오브젝트를 사용
 * 역시나 커널 오브젝트이며 이벤트 오브젝트 생성 함수의 리턴값은 핸들임
 *
 * 이벤트 커널 오브젝트는 프로그래머의 요청에 의해 Signaled 상태가 됨
 * Non Signaled 상태의 이벤트 오브젝트 때문에 WaitForSingleObject 함수 호출이 블로킹 되었다면,
 * Signaled 상태가 되는 순간 블로킹 된 함수를 빠져나오게 된다. 그리고 이때 자동 리셋 모드 이벤트 오브젝트라면,
 * Non Signaled 상태로의 변경이 자동으로 이루어짐
 *
 * 수동 리셋 모드의 이벤트 오브젝트인 경우 ResetEvent 함수를 호출해야만 Non Signaled 상태로 변경
 */

#include <cstdio>
#include <windows.h>
#include <process.h>
#include <tchar.h>

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam);

TCHAR string[100];
HANDLE hEvent;

int _tmain() {
    HANDLE hThread;
    DWORD dwThreadID;

    hEvent = CreateEvent(
        nullptr, // default security attributes
        true, // manual-reset event
        false, // initial state is non-signaled
        nullptr // object name
    );

    if (hEvent == nullptr) {
        _fputts(_T("CreateEvent failed! \n"), stderr);
        return -1;
    }

    // 스레드를 생성하더라도 event 의 상태는 초기값인 Non-Signaled 상태이므로
    // OutputThreadFunction 함수의 WaitForSingleObject 함수 호출 시 블로킹 되어 사용자로부터 입력을 받을때까지 대기
    hThread = (HANDLE) _beginthreadex(nullptr, 0, OutputThreadFunction, nullptr, 0, (unsigned* ) &dwThreadID);
    if (hThread == nullptr) {
        _fputts(_T("Thread creation fault! \n"), stderr);
        return -1;
    }

    _fputts(_T("Insert String: "), stdout);
    _fgetts(string, 30, stdin);

    SetEvent(hEvent); // event 의 state 를 Signaled 상태로 변경
    WaitForSingleObject(hThread, INFINITE); // 스레드 종료 대기
    CloseHandle(hEvent);
    CloseHandle(hThread);

    return 0;
}

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam) {
    // event 가 Signaled 상태가 될 때까지 대기
    WaitForSingleObject(hEvent, INFINITE);
    _fputts(_T("Output String: "), stdout);
    _fputts(string, stdout);
    return 0;
}