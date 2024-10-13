//
// Created by 6712k on 2024-10-13.
//

#include <windows.h>
#include <tchar.h>
#include <cstdio>

#define MAIL_SLOT_NAME _T("\\\\.\\mailslot\\mailbox")

int _tmain(int argc, TCHAR* argv[]) {
    TCHAR messageBox[50];
    DWORD bytesRead;

    HANDLE hMailSlot = CreateMailslot(
        MAIL_SLOT_NAME,
        0, // nMaxMessageSize
        MAILSLOT_WAIT_FOREVER,
        nullptr);
    if (nullptr == hMailSlot) {
        return 1;
    }

    _fputts(_T("************** Message **************\n"), stdout);
    while (1) {
        if (!ReadFile(hMailSlot, messageBox, sizeof(messageBox), &bytesRead, nullptr)) {
            _fputts(_T("ReadFile failed\n"), stdout);
            CloseHandle(hMailSlot);
            return 1;
        }
        if (!_tcsncmp(messageBox, _T("exit"), 4)) {
            break;
        }
        messageBox[bytesRead / sizeof(TCHAR)] = '\0';
        _fputts(messageBox, stdout);
    }
    CloseHandle(hMailSlot);
    return 0;
}