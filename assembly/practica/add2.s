# print(2 + 3)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp

    movq $2, %rax
    pushq %rax
    movq $3, %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax

    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits
