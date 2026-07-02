# print(25-15)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp

    movq $25, %rax
    pushq %rax
    movq $15, %rax
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

