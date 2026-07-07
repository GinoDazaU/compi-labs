# int suma(int a, int b, int c) {
#     int x = a + b + c;
#     return x
#
# int main() {
#     int x = 1;
#     int y = 2;
#     int z = 3;
#     int res = suma(x, y, z);
#     print(res);

.data
__fmt_int: .string "%ld\n"

.text
.globl main

.globl suma

suma:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp

    movq %rdi, %rax
    movq %rax, -8(%rbp)

    movq %rsi, %rax
    movq %rax, -16(%rbp)

    movq %rdx, %rax
    movq %rax, -24(%rbp)

    movq -8(%rbp), %rax
    pushq %rax
    movq -16(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax

    pushq %rax
    movq -24(%rbp), %rax
    movq %rax, %rcx
    popq %rax
    addq %rcx, %rax

    movq %rax, -32(%rbp)
    movq -32(%rbp), %rax
    
    jmp label_endsuma

label_endsuma:
    leave
    ret

main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp

    movq $1, %rax
    movq %rax, -8(%rbp)

    movq $2, %rax
    movq %rax, -16(%rbp)
    
    movq $3, %rax
    movq %rax, -24(%rbp)

    movq -8(%rbp), %rax
    movq %rax, %rdi

    movq -16(%rbp), %rax
    movq %rax, %rsi

    movq -24(%rbp), %rax
    movq %rax, %rdx

    call suma

    movq %rax, %rsi
    movq $0, %rax
    leaq __fmt_int(%rip), %rdi
    call printf@PLT

    movq $0, %rax
    leave
    ret

.section .note.GNU-stack,"",@progbits
