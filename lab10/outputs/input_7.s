.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq $3, %rax
  pushq %rax
  movq $3, %rax
  popq %rcx
  imulq %rcx, %rax
  salq $3, %rax
  movq %rax, %rdi
  call malloc@PLT
  movq %rax, -8(%rbp)
  movq $11, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $22, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $33, %rax
  pushq %rax
  movq $0, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $44, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $55, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $66, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $77, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $88, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $99, %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  popq %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $0, %rax
  movq %rax, -16(%rbp)
  movq $0, %rax
  movq %rax, -32(%rbp)
while_0:
  movq -16(%rbp), %rax
  pushq %rax
  movq $3, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endwhile_0
  movq $0, %rax
  movq %rax, -24(%rbp)
while_1:
  movq -24(%rbp), %rax
  pushq %rax
  movq $3, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endwhile_1
  movq -32(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  movq $3, %rcx
  imulq %rcx, %rax
  addq %rdi, %rax
  movq %rax, %rdi
  movq -8(%rbp), %rax
  movq (%rax, %rdi, 8), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -32(%rbp)
  movq -24(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
  jmp while_1
endwhile_1:
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -16(%rbp)
  jmp while_0
endwhile_0:
  movq -32(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
