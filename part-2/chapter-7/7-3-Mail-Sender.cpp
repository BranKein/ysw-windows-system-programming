//
// Created by 6712k on 2024-10-13.
//

#include <windows.h>
#include <tchar.h>
#include <cstdio>

#define MAIL_SLOT_NAME _T("\\\\.\\mailslot\\mailbox")

int _tmain(int argc, TCHAR* argv[]) {
    HANDLE hMailSlot;
    TCHAR messageBox[50];
    DWORD bytesWritten;

    hMailSlot = CreateFile(
        MAIL_SLOT_NAME,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (INVALID_HANDLE_VALUE == hMailSlot) {
        return 1;
    }

    while (true) {
        _fputts(_T("Input message: "), stdout);
        _fgetts(messageBox, sizeof(messageBox) / sizeof(TCHAR), stdin);

        if (!WriteFile(hMailSlot, messageBox, _tcslen(messageBox) * sizeof(TCHAR), &bytesWritten, nullptr)) {
            _fputts(_T("WriteFile failed\n"), stdout);
            CloseHandle(hMailSlot);
            return 1;
        }

        if (!_tcscmp(messageBox, _T("exit"))) {
            break;
        }
    }
    CloseHandle(hMailSlot);
    return 0;
}