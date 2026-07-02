# x = 1
# i = 1
# n = 10
# while(i < n):
#     print(i)
#     i = i + 1
#     x = x * 2
#     print(x)
# print(i)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp

    movq $1, %rax
    movq %rax, -8(%rbp)

    movq $1, %rax
    movq %rax, -16(%rbp)
    
    movq $10, %rax
    movq %rax, -24(%rbp)

__while_0:
    movq -16(%rbp), %rax
    pushq %rax
    movq -24(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    cmpq %rcx, %rax
    movq $0, %rax
    setl %al
    movzbq %al, %rax

    cmpq $0, %rax
    je __endwhile_0

    movq -16(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq -16(%rbp), %rax
    pushq %rax
    movq $1, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax
    movq %rax, -16(%rbp)

    movq -8(%rbp), %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    imulq %rcx, %rax
    movq %rax, -8(%rbp)

    movq -8(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    jmp __while_0

__endwhile_0:
    movq -16(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits
