.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq $5, %rax
  movq %rax, %rdi
  call dobleInc
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
  leave
  ret

.globl dobleInc
dobleInc:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movq %rdi, -8(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  call inc
  movq %rax, %rcx
  movq $1, %rax
  addq %rcx, %rax
  jmp .end_dobleInc
.end_dobleInc:
  leave
  ret

.globl inc
inc:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movq %rdi, -8(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rcx
  movq $1, %rax
  addq %rcx, %rax
  jmp .end_inc
.end_inc:
  leave
  ret

.section .note.GNU-stack,"",@progbits
