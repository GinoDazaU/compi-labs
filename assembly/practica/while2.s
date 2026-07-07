# i = 1
# n = 8192
# while i < n:
#     print(i)
#     i = i * 2
# print(i < n)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp

    movq $1, %rax
    movq %rax, -8(%rbp)

    movq $8192, %rax
    movq %rax, -16(%rbp)

label_while_0:
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    
    cmpq %rcx, %rax
    movq $0, %rax
    setl %al
    movzbq %al, %rax

    cmpq $0, %rax
    je label_endwhile_0

    movq -8(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq -8(%rbp), %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax

    movq %rax, -8(%rbp)

    jmp label_while_0


label_endwhile_0:
    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movq $0, %rax
    setl %al
    movzbq %al, %rax # el resultado de la operacion binaria, esta en el rax

    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits

