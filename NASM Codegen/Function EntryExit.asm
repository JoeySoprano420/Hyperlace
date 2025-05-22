section .text
global _start

_start:
    call myFunc
    mov rax, 60
    xor rdi, rdi
    syscall

myFunc:
    push rbp
    mov rbp, rsp
    sub rsp, 32              ; allocate local frame

    ; [function body emitted here]

.return:
    mov rsp, rbp
    pop rbp
    ret
