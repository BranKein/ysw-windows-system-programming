//
// Created by 6712k on 2024-10-13.
//

#include <windows.h>
#include <tchar.h>
#include <cstdio>

DWORD _tmain(const int argc, TCHAR* argv[]) {
    if (3 != argc) {
        return -1;
    }

    const DWORD start = _ttoi(argv[1]);
    const DWORD end = _ttoi(argv[2]);

    DWORD total = 0;

    for (auto i = start; i <= end; i++) {
        total += i;
    }

    return total;
}