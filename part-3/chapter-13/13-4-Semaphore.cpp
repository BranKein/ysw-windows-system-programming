/*
* Created by Yeonhyuk on 2024-09-07.
* 세마포어 기반 동기화 (역시 커널 모드 동기화)
*
* 뮤텍스는 가장 단순화된 세마포어 동기화? (바이너리 세마포어) -> 뮤텍스는 세마포어의 일종
* 뮤텍스와 세마포어의 차이는 카운트 기능
* -> 카운트: 동시에 임계영역에 접근 가능한 스레드의 개수를 조절
* -> 뮤텍스는 카운트가 1인 세마포어
*
* 카운트가 0 인 경우 세마포어는 Non-Signaled 상태
* WaitForSingleObject 함수 호출 시 카운트를 하나씩 감소
* 세마포어 오브젝트의 반환은 결국 카운트의 증가를 의미
*
* MyongDongKyoJa.cpp
* 목적: 카운트 세마포어에 대한 이해
* 시뮬레이션 제한요소
* 1. 테이블이 총 10개이고, 동시에 총 10분의 손님만 받을 수 있다고 가정
* 2. 오늘 점심시간에 식사하러 오실 예상 손님 수는 총 50분
* 3. 각 손님들께서 식사하시는 시간은 대략 10분에서 30분 사이
*
*/

#include <cstdio>
#include <ctime>
#include <process.h>
#include <tchar.h>
#include <windows.h>

#define NUM_OF_CUSTOMER 50
#define RANGE_MIN 10
#define RANGE_MAX (30 - RANGE_MIN)
#define TABLE_CNT 10

HANDLE hSemaphore;
DWORD randTimeArr[NUM_OF_CUSTOMER];

void TakeMeal(const DWORD time) {
    DWORD tId = GetCurrentThreadId();

    WaitForSingleObject(hSemaphore, INFINITE);
    _tprintf(_T("Enter Customer %d~ \n"), tId);

    _tprintf(_T("Customer %d having launch~ \n"), tId);
    Sleep(1000 * time);

    ReleaseSemaphore(hSemaphore, 1, nullptr);
    _tprintf(_T(("Out Customer %d~ \n")), tId);
}

unsigned int WINAPI CustomerProc(LPVOID lpParam) {
    TakeMeal((DWORD) lpParam);
    return 0;
}

int _tmain(int argc, TCHAR *argv[]) {
    DWORD dwThreadIds[NUM_OF_CUSTOMER];;
    HANDLE hThread[NUM_OF_CUSTOMER];

    // random seed
    srand((unsigned) time(nullptr));

    for (int i = 0; i < NUM_OF_CUSTOMER; i++) {
        randTimeArr[i] = (DWORD) (((double) rand() / (double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
    }

    hSemaphore = CreateSemaphore(
        nullptr, // default security
        TABLE_CNT, // initial count
        TABLE_CNT, // maximum count
        nullptr // unnamed semaphore
    );

    if (hSemaphore == nullptr) {
        _tprintf(_T("Semaphore creation fault! \nerrror: %d\n"), GetLastError());
        return -1;
    }

    for (int i = 0; i < NUM_OF_CUSTOMER; i++) {
        hThread[i] = (HANDLE) _beginthreadex(
            nullptr,
            0,
            CustomerProc,
            (LPVOID) randTimeArr[i],
            CREATE_SUSPENDED,
            (unsigned *) &dwThreadIds[i]
        );
        if (hThread[i] == nullptr) {
            _tprintf(_T("Thread creation fault! \n"));
            return -1;
        }
    }

    for (int i = 0; i < NUM_OF_CUSTOMER; i++) {
        ResumeThread(hThread[i]);
    }

    WaitForMultipleObjects(NUM_OF_CUSTOMER, hThread, true, INFINITE);

    _tprintf(_T("---------------END---------------\n"));

    for (int i = 0; i < NUM_OF_CUSTOMER; i++) {
        CloseHandle(hThread[i]);
    }
    CloseHandle(hSemaphore);

    return 0;
}
