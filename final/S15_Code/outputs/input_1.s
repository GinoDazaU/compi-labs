.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $24, %rsp
  movq $3, %rax
  movq %rax, %rcx
  movq $5, %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  movq %rax, -8(%rbp)
  movq $2, %rax
  movq %rax, %rcx
  movq $4, %rax
  addq %rcx, %rax
  movq %rax, -16(%rbp)
  movq $10, %rax
  movq %rax, %rcx
  movq $3, %rax
  imulq %rcx, %rax
  movq %rax, -24(%rbp)
  movq $0, %rax
  jmp .end_main
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
