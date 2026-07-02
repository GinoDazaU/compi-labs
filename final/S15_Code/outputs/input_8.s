.data
print_fmt: .string "%ld \n"

.text

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $0, %rsp
  movq $3, %rax
  movq %rax, %rdi
  call doble
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $4, %rax
  movq %rax, %rdi
  movq $5, %rax
  movq %rax, %rsi
  call suma
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $2, %rax
  movq %rax, %rdi
  movq $3, %rax
  movq %rax, %rsi
  call suma
  movq %rax, %rdi
  call doble
  movq %rax, %rsi
  leaq print_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $0, %rax
  jmp .end_main
.end_main:
  leave
  ret

.globl doble
doble:
  pushq %rbp
  movq %rsp, %rbp
  subq $8, %rsp
  movq %rdi, -8(%rbp)
  movq $2, %rax
  movq %rax, %rcx
  movq -8(%rbp), %rax
  imulq %rcx, %rax
  jmp .end_doble
.end_doble:
  leave
  ret

.globl suma
suma:
  pushq %rbp
  movq %rsp, %rbp
  subq $16, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq -8(%rbp), %rax
  movq %rax, %rcx
  movq -16(%rbp), %rax
  addq %rcx, %rax
  jmp .end_suma
.end_suma:
  leave
  ret

.section .note.GNU-stack,"",@progbits
