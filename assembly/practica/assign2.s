# x = 3
# y = 2
# z = x + y
# print(z)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp

    movq $3, %rax
    movq %rax, -8(%rbp)

    movq $2, %rax
    movq %rax, -16(%rbp)

    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax

    movq %rax, -24(%rbp)

    movq -24(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits

