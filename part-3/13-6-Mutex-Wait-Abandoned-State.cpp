/*
 * Created by Yeonhyuk on 2024-09-07.
 * WAIT_ABANDONED 상태 생성
 *
 * 뮤텍스는 카운트를 감소시킨 스레드 (해당 뮤텍스를 사용하고 있는 스레드) 가 반드시 뮤텍스를 반환해야 한다.
 * 하지만 세마포어의 경우 다른 스레드가 오브젝트를 반환할 수 있다.
 * 만약 스레드가 뮤텍스를 반환하지 않고 사라져버릴 경우 WAIT_ABANDONED 상태가 발생한다.
 *
 * Windows 는 스레드와 뮤텍스의 상태를 지속적으로 감시하고 있기 때문에 정상적으로 반환하지 못하는 뮤텍스가 있고,
* 이 뮤텍스를 사용하려고 할 경우 WAIT_ABANDONED 상태를 반환한다.
*/

#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <process.h>

LONG gTotalCount = 0;
HANDLE hMutex;

unsigned int WINAPI IncreaseCountOne(LPVOID lpParam) {
    WaitForSingleObject(hMutex, INFINITE);
    gTotalCount++;

    // Did not release mutex
    // make hMutex in WAIT_ABANDONED state

    return 0;
}

unsigned int WINAPI IncreaseCountTwo(LPVOID lpParam) {
    DWORD dwWaitResult = 0;

    dwWaitResult = WaitForSingleObject(hMutex, INFINITE);

    switch (dwWaitResult) {
        case WAIT_OBJECT_0:
            ReleaseMutex(hMutex);
            break;
        case WAIT_ABANDONED:
            _tprintf(_T("Thread got abandoned mutex! \n"));
            break;
    }

    gTotalCount++;

    ReleaseMutex(hMutex);

    return 0;
}

int _tmain() {
    DWORD dwThreadIdOne;
    DWORD dwThreadIdTwo;

    HANDLE hThreadOne;
    HANDLE hThreadTwo;

    hMutex = CreateMutex(nullptr, false, nullptr);

    if (hMutex == nullptr) {
        _tprintf(_T("CreateMutex error: %d \n"), GetLastError());
        return -1;
    }

    hThreadOne = (HANDLE) _beginthreadex(nullptr, 0, IncreaseCountOne, nullptr, 0, (unsigned *) &dwThreadIdOne);
    hThreadTwo = (HANDLE) _beginthreadex(nullptr, 0, IncreaseCountTwo, nullptr, CREATE_SUSPENDED, (unsigned *) &dwThreadIdTwo);

    Sleep(1000);

    ResumeThread(hThreadTwo);

    WaitForSingleObject(hThreadTwo, INFINITE);

    _tprintf(_T("Total Count: %d \n"), gTotalCount);

    CloseHandle(hThreadOne);
    CloseHandle(hThreadTwo);
    CloseHandle(hMutex);

    return 0;

}
