# x = 10
# y = 20
# if x > y:
#     print(x)
# else:
#     print(y)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp

    movq $10, %rax
    movq %rax, -8(%rbp)

    movq $20, %rax
    movq %rax, -16(%rbp)

    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax

    cmpq %rcx, %rax
    movq $0, %rax
    setg %al
    movzbq %al, %rax

    cmpq $0, %rax
    je __else_0

    movq -8(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT
    jmp __endif_0

__else_0:
    movq -16(%rbp), %rax
    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT
    jmp __endif_0

__endif_0:
    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits
