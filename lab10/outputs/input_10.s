.data
print_fmt: .string "%ld \n"

.text

.globl sumaLista
sumaLista:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq $0, %rax
  movq %rax, -24(%rbp)
  movq $0, %rax
  movq %rax, -32(%rbp)
while_0:
  movq -24(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endwhile_0
  movq -32(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
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
  jmp while_0
endwhile_0:
  movq -32(%rbp), %rax
  jmp .end_sumaLista
.end_sumaLista:
  leave
  ret

.globl mayorLista
mayorLista:
  pushq %rbp
  movq %rsp, %rbp
  subq $32, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq $1, %rax
  movq %rax, -24(%rbp)
  movq $0, %rax
  movq %rax, %rdi
  movq -8(%rbp), %rax
  movq (%rax, %rdi, 8), %rax
  movq %rax, -32(%rbp)
while_1:
  movq -24(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endwhile_1
  movq -24(%rbp), %rax
  movq %rax, %rdi
  movq -8(%rbp), %rax
  movq (%rax, %rdi, 8), %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setg %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_2
  movq -24(%rbp), %rax
  movq %rax, %rdi
  movq -8(%rbp), %rax
  movq (%rax, %rdi, 8), %rax
  movq %rax, -32(%rbp)
  jmp endif_2
else_2:
endif_2:
  movq -24(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
  jmp while_1
endwhile_1:
  movq -32(%rbp), %rax
  jmp .end_mayorLista
.end_mayorLista:
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq $4, %rax
  movq $8, %rcx
  imulq %rcx, %rax
  movq %rax, %rdi
  call malloc@PLT
  movq %rax, -8(%rbp)
  movq $4, %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rdi
  popq %rax
  movq %rax, %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $8, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  movq %rax, %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $3, %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rdi
  popq %rax
  movq %rax, %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $9, %rax
  pushq %rax
  movq $3, %rax
  movq %rax, %rdi
  popq %rax
  movq %rax, %rcx
  movq -8(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $16, %rdi
  call malloc@PLT
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  movq $4, %rax
  movq %rax, %rsi
  call sumaLista
  pushq %rax
  movq $0, %rax
  movq %rax, %rdi
  popq %rax
  movq %rax, %rcx
  movq -16(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq -8(%rbp), %rax
  movq %rax, %rdi
  movq $4, %rax
  movq %rax, %rsi
  call mayorLista
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  movq %rax, %rcx
  movq -16(%rbp), %rax
  movq %rcx, (%rax, %rdi, 8)
  movq $0, %rdi
  movq -16(%rbp), %rax
  movq (%rax, %rdi, 8), %rax
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $1, %rdi
  movq -16(%rbp), %rax
  movq (%rax, %rdi, 8), %rax
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
