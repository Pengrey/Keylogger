#include <windows.h>
#include <stdio.h>

#define MIN_UPTIME 300000 // 5 min; 5*60=300s; 5*60*1000=300000 ms

//Check for Computer Uptime Greater than 5 min
int checkUptime() {
    // System uptime
    // We are using GetTickCount64
    // Retrieves the number of milliseconds that have elapsed since the system was started.
    // https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount64
    return (GetTickCount() < (MIN_UPTIME));
}