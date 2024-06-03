section .text
    global NtUserGetAsyncKeyState

NtUserGetAsyncKeyState:
    mov r10, rcx
    mov eax, [[SSN]]

    syscall
    ret