//
// Created by 6712k on 2024-09-18.
//

#include <windows.h>
#include <tchar.h>
#include <cstdio>

#define WORK_MAX 10000
#define THREAD_MAX 10

typedef void (*WORK) ();

typedef struct __WorkThread {
    HANDLE hThread;
    DWORD idThread;
} WorkThread;

struct __ThreadPool {
    // Work 를 등록하기 위한 배열
    WORK workList[WORK_MAX];

    // Thread 정보와 각 Thread 별 Event 오브젝트
    WorkThread workerThreadList[THREAD_MAX];
    HANDLE workerEvnetList[THREAD_MAX];

    // Work 에 대한 정보
    DWORD idxOfCurrentWork; // 대기 1순위 Work Index
    DWORD idxOfLastAddedWork; // 마지막 추가 Work Index + 1

    // Number of Thread;
    DWORD threadIdx; // Pool 에 존재하는 Thread 의 개수
} gThreadPool;

// 쓰레드 풀에서 Work 를 가져올 때 호출
WORK GetWorkFromPool();
// 새로운 Work 를 등록할 때 호출
DWORD AddWorkToPool(WORK work);
// 쓰레드 풀이 생성된 이후에 풀에 쓰레드를 등록하는 함수. 인자로 전달되는 수만큼 쓰레드를 생성
DWORD MakeThreadToPool(DWORD numOfThread);
// 쓰레드가 생성되자마자 호출하는 쓰레드의 main 함수. lpParam 은 쓰레드의 인덱스
void WorkerThreadFunction(LPVOID lpParam);

// ************************************************************************************** //
// Mutex 관련 함수들
// gThreadPool 에 대한 접근을 보호하기 위해 정의
// ************************************************************************************** //

static HANDLE mutex = NULL;

void InitMutex() {
    mutex = CreateMutex(nullptr, false, nullptr);
}

void DeInitMutex() {
    BOOL ret = CloseHandle(mutex);
}

void AcquireMutex() {
    DWORD ret = WaitForSingleObject(mutex, INFINITE);
    if (ret == WAIT_FAILED)
        _tprintf(_T("Error Occur! \n"));
}

void ReleaseMutex() {
    BOOL ret = ReleaseMutex(mutex);
    if (ret == FALSE)
        _tprintf(_T("Error Occur! \n"));
}

// ************************************************************************************** //
// Thread Pool 에 Work 를 등록시키기 위한 함수
// ************************************************************************************** //

DWORD AddWorkToPool(WORK work) {
    AcquireMutex();
    if (gThreadPool.idxOfLastAddedWork >= WORK_MAX) {
        _tprintf(_T("AddWorkToPool Failed! \n"));
        return NULL;
    }

    gThreadPool.workList[gThreadPool.idxOfLastAddedWork++] = work;

    for (DWORD i = 0; i < gThreadPool.threadIdx; i++) {
        SetEvent(gThreadPool.workerEvnetList[i]);
    }

    ReleaseMutex();
    return 1;
}

WORK GetWorkFromPool() {
    WORK work = nullptr;
    AcquireMutex();

    // 남아있는 Work 가 없다면 NULL 반환
    if (gThreadPool.idxOfCurrentWork >= gThreadPool.idxOfLastAddedWork) {
        ReleaseMutex();
        return nullptr;
    }

    work = gThreadPool.workList[gThreadPool.idxOfCurrentWork++];
    ReleaseMutex();
    return work;
}

DWORD MakeThreadToPool(DWORD numOfThread) {
    InitMutex();
    DWORD capacity = WORK_MAX - gThreadPool.threadIdx;
    if (capacity < numOfThread) {
        numOfThread = capacity;
    }

    for (DWORD i=0; i < numOfThread; i++) {
        DWORD idThread;
        HANDLE hThread;

        gThreadPool.workerEvnetList[gThreadPool.threadIdx] = CreateEvent(nullptr, false, false, nullptr);
        hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)WorkerThreadFunction, (LPVOID)gThreadPool.threadIdx, 0, &idThread);

        gThreadPool.workerThreadList[gThreadPool.threadIdx].hThread = hThread;
        gThreadPool.workerThreadList[gThreadPool.threadIdx].idThread = idThread;

        gThreadPool.threadIdx++;
    }

    return numOfThread;
}

void WorkerThreadFunction(LPVOID lpParam){
    WORK workFunction;
    HANDLE event = gThreadPool.workerEvnetList[(DWORD)lpParam]; // 해당 쓰레드의 Event

    while (true) {
        workFunction = GetWorkFromPool();
        if (workFunction == nullptr) {
            WaitForSingleObject(event, INFINITE);
            continue;
        }
        workFunction();
    }
}

void TestFunction() {
    static int i = 0; // 동기화가 필요하지만 생략
    i++;

    _tprintf(_T("Good Test --%d : Processing thread : %d--\n\n"), i, GetCurrentThreadId());
}

int _tmain(int argc, TCHAR* argv[]) {
    MakeThreadToPool(3);

    for (int i = 0; i < 1000; i++) {
        AddWorkToPool(TestFunction);
    }

    Sleep(50000);
    DeInitMutex();

    return 0;
}

