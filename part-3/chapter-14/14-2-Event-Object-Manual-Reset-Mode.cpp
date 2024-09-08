/*
 * Created by Yeonhyuk on 2024-09-08.
 * 수동 리셋 모드의 이벤트 기반 스레드
 *
 * 이전 14-1 과 다르게 이번엔 소비자 측의 스레드가 하나 더 추가
 * 입력받은 문자열을 그대로 출력하는 스레드 하나와 문자열의 길이를 계산하여 출력하는 스레드를 추가
 *
 * 만약 자동 리셋 모드의 경우 Signaled 상태가 된 event 오브젝트를 OutputThread 또는 LengthThread 중 하나에서
 * 블로킹의 해제와 함께 Non-Signaled 상태로 변경해버리기 때문에 다른 한쪽은 작동을 안할 수 있음
 * 때문에 수동 리셋 모드로 설정하여 자동으로 다시 Non Signaled 상태로 변경되지 않도록 사용함
 */

#include <cstdio>
#include <windows.h>
#include <process.h>
#include <tchar.h>

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam);
unsigned int WINAPI LengthThreadFunction(LPVOID lpParam);

TCHAR string[100];
HANDLE hEvent;

int _tmain() {
    HANDLE hThreadPrint, hThreadLength;
    DWORD dwThreadIDPrint, dwThreadIDLength;

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
    hThreadPrint = (HANDLE) _beginthreadex(nullptr, 0, OutputThreadFunction, nullptr, 0, (unsigned* ) &dwThreadIDPrint);
    if (hThreadPrint == nullptr) {
        _fputts(_T("Thread creation fault! \n"), stderr);
        return -1;
    }

    hThreadLength = (HANDLE) _beginthreadex(nullptr, 0, LengthThreadFunction, nullptr, 0, (unsigned* ) &dwThreadIDLength);
    if (hThreadLength == nullptr) {
        _fputts(_T("Thread creation fault! \n"), stderr);
        return -1;
    }

    _fputts(_T("Insert String: "), stdout);
    _fgetts(string, 30, stdin);

    SetEvent(hEvent); // event 의 state 를 Signaled 상태로 변경
    WaitForMultipleObjects(2, new HANDLE[2]{hThreadPrint, hThreadLength}, TRUE, INFINITE); // 스레드 종료 대기

    CloseHandle(hEvent);
    CloseHandle(hThreadPrint);
    CloseHandle(hThreadLength);

    return 0;
}

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam) {
    // event 가 Signaled 상태가 될 때까지 대기
    WaitForSingleObject(hEvent, INFINITE);
    _fputts(_T("Output String: "), stdout);
    _fputts(string, stdout);
    return 0;
}

unsigned int WINAPI LengthThreadFunction(LPVOID lpParam) {
    // event 가 Signaled 상태가 될 때까지 대기
    WaitForSingleObject(hEvent, INFINITE);
    _fputts(_T("Output Length: "), stdout);
    _tprintf(_T("%llu \n"), _tcslen(string) - 1);
    return 0;
}