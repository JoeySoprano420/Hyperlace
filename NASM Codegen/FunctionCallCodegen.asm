    mov rdi, 5          ; push arg 1
    mov rsi, 7          ; push arg 2
    call myFunc
    mov [z], rax        ; capture return value
