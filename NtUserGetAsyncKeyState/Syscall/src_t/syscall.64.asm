section .text
    global NtUserGetAsyncKeyState

NtUserGetAsyncKeyState:
    mov r10, rcx
    mov eax, dword [[SSN]]

    syscall
    ret