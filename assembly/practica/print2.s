# print(19)
# print(100)

.data
__fmt_int: .string "%ld\n"

.text
.globl main

main:
    pushq %rbp
    movq %rsp, %rbp

    movq $19, %rax
    movq %rax, %rsi
    movq $0, %rax

    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $100, %rax
    movq %rax, %rsi
    movq $0, %rdi
    leaq __fmt_int(%rip), %rdi
    call printf@PLT
    
    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits
