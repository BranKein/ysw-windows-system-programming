/*
 * Created by Yeonhyuk on 2024-09-08.
 * 이벤트 오브젝트와 뮤텍스의 짬뽕
 *
 * 이전 14-2 의 경우 OutputThread 와 LengthThread 가 같이 실행되기는 하지만 순서 보장이 되지 않음
 * 때문에 뮤텍스를 순차적으로 Wait 하게 만들어서 순서를 보장시킴
 *
 *
 *
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

typedef struct _SyncString {
    TCHAR string[100];
    HANDLE hEvent;
    HANDLE hMutex;
} SyncString;

SyncString syncString;

int _tmain() {
    HANDLE hThreadPrint, hThreadLength;
    DWORD dwThreadIDPrint, dwThreadIDLength;

    syncString.hEvent = CreateEvent(
        nullptr, // default security attributes
        true, // manual-reset event
        false, // initial state is non-signaled
        nullptr // object name
    );

    syncString.hMutex = CreateMutex(nullptr, false, nullptr);

    if (syncString.hEvent == nullptr || syncString.hMutex == nullptr) {
        _fputts(_T("CreateEvent failed! \n"), stderr);
        return -1;
    }

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
    _fgetts(syncString.string, 30, stdin);

    SetEvent(syncString.hEvent); // event 의 state 를 Signaled 상태로 변경
    WaitForMultipleObjects(2, new HANDLE[2]{hThreadPrint, hThreadLength}, TRUE, INFINITE); // 스레드 종료 대기

    CloseHandle(syncString.hEvent);
    CloseHandle(syncString.hMutex);
    CloseHandle(hThreadPrint);
    CloseHandle(hThreadLength);

    return 0;
}

unsigned int WINAPI OutputThreadFunction(LPVOID lpParam) {
    // event 가 Signaled 상태가 될 때까지 대기
    WaitForSingleObject(syncString.hEvent, INFINITE);
    WaitForSingleObject(syncString.hMutex, INFINITE);

    _fputts(_T("Output String: "), stdout);
    _fputts(syncString.string, stdout);

    ReleaseMutex(syncString.hMutex);
    return 0;
}

unsigned int WINAPI LengthThreadFunction(LPVOID lpParam) {
    // event 가 Signaled 상태가 될 때까지 대기
    WaitForSingleObject(syncString.hEvent, INFINITE);
    WaitForSingleObject(syncString.hMutex, INFINITE);
    _fputts(_T("Output Length: "), stdout);
    _tprintf(_T("%llu \n"), _tcslen(syncString.string) - 1);

    ReleaseMutex(syncString.hMutex);
    return 0;
}