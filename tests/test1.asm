section .data
STDOUT      equ     1
sys_write   equ     1
sys_exit    equ     60
msg         db      "filename=tests/test1", 10, 0
msgLen      dq      ($ - msg)
section .text
global _start
_start:
    mov     rax, sys_write
    mov     rdi, STDOUT
    mov     rsi, msg
    mov     rdx, qword [msgLen]
    syscall
    jmp exit
exit:
    mov     rax, sys_exit
    xor     rdi, rdi
    syscall
