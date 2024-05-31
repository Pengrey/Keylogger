section .text
    global _NtUserGetAsyncKeyState

_NtUserGetAsyncKeyState:
    mov ecx, [[SSN]]
    mov edx, [esp + 4]
    int 0x2e
    ret