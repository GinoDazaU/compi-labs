# print(10 - 2)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp

    movq $10, %rax
    pushq %rax
    movq $2, %rax
    movq %rax, %rcx
    popq %rax
    subq %rcx, %rax

    movq %rax, %rsi
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits
