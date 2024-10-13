//
// Created by 6712k on 2024-10-13.
//

#include <windows.h>
#include <tchar.h>
#include <cstdio>

int _tmain(const int argc, TCHAR* argv[]) {
    STARTUPINFO si1 = {0,};
    STARTUPINFO si2 = {0,};

    PROCESS_INFORMATION pi1;
    PROCESS_INFORMATION pi2;

    DWORD return_val1 = 0;
    DWORD return_val2 = 0;

    TCHAR commandLine1[] = _T("part-2-7-4.exe 1 5000");
    TCHAR commandLine2[] = _T("part-2-7-4.exe 5001 10000");

    DWORD sum = 0;

    si1.cb = sizeof(si1);
    si2.cb = sizeof(si2);
    CreateProcess(nullptr, commandLine1, nullptr, nullptr, true, 0, nullptr, nullptr, &si1, &pi1);
    CreateProcess(nullptr, commandLine2, nullptr, nullptr, true, 0, nullptr, nullptr, &si2, &pi2);

    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hThread);

    // WaitForSingleObject(pi1.hProcess, INFINITE);
    // WaitForSingleObject(pi2.hProcess, INFINITE);

    const HANDLE handles[] = {pi1.hProcess, pi2.hProcess};

    WaitForMultipleObjects(2, handles, true, INFINITE);

    // 프로세스가 종료되기도 전에 exit code 를 가져오려 하기 때문에 STILL_ACTIVE 에 해당하는 259 가 리턴됨
    GetExitCodeProcess(pi1.hProcess, &return_val1);
    GetExitCodeProcess(pi2.hProcess, &return_val2);

    if (-1 == return_val1 || -1 == return_val2) {
        return -1;
    }

    sum += return_val1;
    sum += return_val2;

    _tprintf(_T("Total: %d\n"), sum);
    _tprintf(_T("Expected Total: %d\n"), (1 + 10000) * 10000 / 2);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi2.hProcess);

    return 0;
}
