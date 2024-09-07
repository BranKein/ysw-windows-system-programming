/*
 * Created by Yeonhyuk on 2024-09-06.
 * 크리티컬 섹션 기반 동기화
 *
 * CRITICAL_SECTION 을 사용하여 Enter, Leave 사이의 임계영역을 생성
 * 크리티컬 섹션 사용 전 Initialize 함수를 사용하여 초기화
 * 사용 후에는 Delete 함수를 사용하여 삭제
 */


#include <cstdio>
#include <windows.h>
#include <process.h>
#include <tchar.h>


#define NUM_OF_GATE 6

LONG gTotalCount = 0;

CRITICAL_SECTION hCriticalSection;

void IncreaseCount() {
    EnterCriticalSection(&hCriticalSection);
    gTotalCount++;
    LeaveCriticalSection(&hCriticalSection);
}

unsigned int WINAPI ThreadProc(LPVOID lpParam) {
    for (DWORD i = 0; i < 1000; i++) {
        IncreaseCount();
    }
    return 0;
}

int _tmain(int argc, TCHAR *argv[]) {
    DWORD dwThreadId[NUM_OF_GATE];
    HANDLE hThread[NUM_OF_GATE];

    InitializeCriticalSection(&hCriticalSection);

    for (DWORD i = 0; i < NUM_OF_GATE; i++) {
        hThread[i] = (HANDLE) _beginthreadex(nullptr, 0, ThreadProc, nullptr, CREATE_SUSPENDED,
                                             reinterpret_cast<unsigned *>(&dwThreadId[i]));
        if (hThread[i] == nullptr) {
            _tprintf(_T("Thread creation fault! \n"));
            return -1;
        }
    }

    for (DWORD i = 0; i < NUM_OF_GATE; i++) {
        ResumeThread(hThread[i]);
    }

    WaitForMultipleObjects(NUM_OF_GATE, hThread, TRUE, INFINITE);
    _tprintf(_T("total count: %d \n"), gTotalCount);

    for (DWORD i = 0; i < NUM_OF_GATE; i++) {
        CloseHandle(hThread[i]);
    }

    DeleteCriticalSection(&hCriticalSection);

    return 0;
}
