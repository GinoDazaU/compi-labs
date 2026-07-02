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
  call triple
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
  leave
  ret

.globl triple
triple:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movq %rdi, -8(%rbp)
  movq $3, %rax
  movq %rax, %rcx
  movq -8(%rbp), %rax
  imulq %rcx, %rax
  jmp .end_triple
.end_triple:
  leave
  ret

.section .note.GNU-stack,"",@progbits
