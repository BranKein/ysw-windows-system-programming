/*
 * Created by Yeonhyuk on 2024-09-07.
 * 뮤텍스 기반 스레드 동기화
 *
 * 뮤텍스가 누군가에 의해 획득이 가능할 때 Signaled 상태가 된다
 * 누군가가 획득을 하고 나면 Non-Signaled 상태로 변경
 *
 * 뮤텍스는 생성과 동시에 초기화 -> 리턴타입은 핸들 -> 즉 뮤텍스 오브젝트는 커널 오브젝트임을 말함
 * 뮤텍스는 커널 오브젝트이기 때문에 커널 모드에서 동작
 * 뮤텍스 오브젝트 리소스 해제를 위해서는 역시 뮤텍스는 커널 오브젝트이니 CloseHandle 함수를 사용
 *
 * 스레드 입장에서는 뮤텍스를 획득하고자 할 때 Wait 함수를 사용하여 획득을 시도
 * 획득에 성공하면 해당 커널 오브젝트의 상태를 Non Signaled 로 변경 -> 다른 스레드들이 획득을 못하게 방지
 * 획득에 성공한 스레드가 임계영역을 빠져나올 때 ReleaseMutex 함수 호출 -> 상태를 Signaled 로 변경
 */

#include <cstdio>
#include <windows.h>
#include <process.h>
#include <tchar.h>


#define NUM_OF_GATE 6

LONG gTotalCount = 0;

// CRITICAL_SECTION hCriticalSection;
HANDLE hMutex;

void IncreaseCount() {
    // hMutex 의 획득을 시도, 즉 Signaled 상태의 뮤텍스를 획득하고 Non-Signaled 상태로 변경
    WaitForSingleObject(hMutex, INFINITE);

    gTotalCount++;

    // hMutex 의 상태를 Signaled 로 변경하여 다른 스레드가 사용할 수 있도록 변경
    ReleaseMutex(hMutex);
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

    // 뮤텍스 생성과 동시에 초기화, 리턴타입은 핸들
    hMutex = CreateMutex(nullptr, false, nullptr);

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

    // 뮤텍스 오브젝트 리소스 해제
    CloseHandle(hMutex);

    return 0;
}
