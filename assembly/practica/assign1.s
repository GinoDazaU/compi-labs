# x = 6
# y = 10
# print(x)
# print(y)
# print(x + y)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    
    movq $6, %rax
    movq %rax, -8(%rbp)

    movq $10, %rax
    movq %rax, -16(%rbp)

    movq -8(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq -16(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
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
