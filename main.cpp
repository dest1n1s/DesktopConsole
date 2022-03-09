#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <thread>
#include <chrono>

using namespace std;
HWND hTerminal, hWallpaper, hIcon;
HHOOK hKeyboard;
HWND hInput;

BOOL CALLBACK EWFindWallpaper(HWND hwnd, [[maybe_unused]] LPARAM lParam) {
    HWND p = FindWindowEx(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);

    if (p) {
        hIcon = hwnd;
        // Gets the WorkerW  Window after the current one.
        hWallpaper = FindWindowEx(nullptr, hwnd, L"WorkerW", nullptr);
        return FALSE;
    }
    return TRUE;

}

BOOL CALLBACK EWFindTerminal(HWND hwnd, LPARAM lParam) {
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    TCHAR className[512] = _T(""), title[512] = _T("");
    GetClassName(hwnd, className, 512);
    GetWindowText(hwnd, title, 512);
    wprintf(L"%d %d %s %s\n", lpdwProcessId, hwnd, className, title);
    if (lpdwProcessId == lParam) {
        hTerminal = hwnd;
        return FALSE;
    }
    EnumChildWindows(hwnd, EWFindTerminal, lParam);
    return TRUE;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0)return CallNextHookEx(hKeyboard, nCode, wParam, lParam);
    KBDLLHOOKSTRUCT kInfo = *(PKBDLLHOOKSTRUCT) lParam;
    HWND hForeground = GetForegroundWindow();
    if (hForeground == hIcon)
        switch (wParam) {
            case WM_KEYDOWN:
            case WM_KEYUP:
                PostMessage(hInput, wParam, kInfo.vkCode, kInfo.flags << 24 | kInfo.scanCode << 16 | 1);
                return 1;
            default:
                break;
        }
    else {
        cout << hForeground << endl;
    }
    return CallNextHookEx(hKeyboard, nCode, wParam, lParam);
}

int main() {
    setlocale(LC_ALL, "chs");
    HWND progman = FindWindow(_T("Progman"), nullptr);
    PDWORD_PTR result = nullptr;
    SendMessageTimeout(progman,
                       0x052C,
                       NULL,
                       NULL,
                       SMTO_NORMAL,
                       1000,
                       result);
    EnumWindows(EWFindWallpaper, NULL);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);
    LPTSTR cmdLine = _tcsdup(TEXT("wt"));
    CreateProcess(nullptr,
                  cmdLine,
                  nullptr,
                  nullptr,
                  FALSE, 0,
                  nullptr,
                  nullptr,
                  &si,
                  &pi);
    Sleep(1000);
    //EnumWindows(EWFindTerminal, pi.dwProcessId);
    hTerminal = FindWindow(_T("CASCADIA_HOSTING_WINDOW_CLASS"), nullptr);
    HWND hContent = FindWindowEx(hTerminal, nullptr, _T("Windows.UI.Composition.DesktopWindowContentBridge"), nullptr);
    hInput = FindWindowEx(hContent, nullptr, _T("Windows.UI.Input.InputSite.WindowClass"), nullptr);
    SetParent(hContent, hWallpaper);

    // SetFocus(hTerminal);

    hKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {

    }
}