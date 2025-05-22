sub rsp, 32       ; space for local vars x, y, result
mov qword [rbp-8], 5
mov rax, [rbp-8]
mov [rbp-16], rax
