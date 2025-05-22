_start:
    call sumUp
    mov rax, 60
    xor rdi, rdi
    syscall

sumUp:
    push rbp
    mov rbp, rsp
    sub rsp, 32
    ; ...
    mov rsp, rbp
    pop rbp
    ret
