/*
 * Created by Yeonhyuk on 2024-09-07.
 * 네임드 뮤텍스의 이해
 *
 */

#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <process.h>

HANDLE hMutex;
DWORD dwWaitResult;

void ProcessBaseCriticalSection() {
    dwWaitResult = WaitForSingleObject(hMutex, INFINITE);

    switch (dwWaitResult) {
        // 스레드가 뮤텍스를 소유
        case WAIT_OBJECT_0:
            _tprintf(_T("Thread got mutex! \n"));
            break;

        // time out 발생
        case WAIT_TIMEOUT:
            _tprintf(_T("Thread got timeout! \n"));
            return;

        // 뮤텍스 반환이 적절히 이루어지지 않음
        case WAIT_ABANDONED:
            return;
    }

    for (DWORD i=0; i<5; i++) {
        _tprintf(_T("Thread %d is working... \n"), GetCurrentThreadId());
        Sleep(10000);
    }

    ReleaseMutex(hMutex);
}

int _tmain(int argc, TCHAR* argv[]) {
    // 조건부 컴파일: if 부분에 1 이 들어가면 CreateMutex, 0 이 들어가면 OpenMutex
#if 0
    hMutex = CreateMutex(nullptr, FALSE, _T("NamedMutex"));
#else
    hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("NamedMutex"));
#endif
    if (hMutex == nullptr) {
        _tprintf(_T("CreateMutex error: %d \n"), GetLastError());
        return 1;
    }

    ProcessBaseCriticalSection();

    CloseHandle(hMutex);
    return 0;
}
