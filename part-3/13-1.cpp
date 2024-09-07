/*
 * Created by Yeonhyuk on 2024-09-06.
 */


#include <cstdio>
#include <windows.h>
#include <process.h>
#include <tchar.h>


#define NUM_OF_GATE 6

LONG gTotalCount = 0;

void IncreaseCount() {
    gTotalCount++;
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

    return 0;
}

