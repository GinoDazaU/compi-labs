# Implementación de Potencia Rápida en el Compilador

En lugar de un bucle lineal, se emite una función auxiliar `potencia` en la sección `.text` que implementa exponenciación por cuadrado (fast exponentiation). Cada vez que el generador encuentra el operador `**`, emite una llamada a esa función en lugar de código inline.

---

## 1. Idea de Alto Nivel (Algoritmo Recursivo)

El algoritmo reduce el problema a la mitad en cada llamada:

$$
\text{potencia}(x, n) =
\begin{cases}
1 & \text{si } n = 0 \\
x & \text{si } n = 1 \\
\text{potencia}(x \times x,\ n \div 2) & \text{si } n \text{ es par} \\
\text{potencia}(x \times x,\ n \div 2) \times x & \text{si } n \text{ es impar}
\end{cases}
$$

### Convención de llamada usada
- `%rdi` → argumento `x` (base)
- `%rsi` → argumento `n` (exponente)
- `%rax` → valor de retorno

---

## 2. Flujo de Operaciones y Aritmética x86-64

### A. Emisión de la función auxiliar (`visit(Program)`)

La función se emite **una sola vez** al inicio de la sección `.text`, antes de las funciones del usuario.

```
emit(".text")
emit("potencia:")
  prologo

  si %rsi == 0: %rax = 1, retornar
  si %rsi == 1: %rax = %rdi, retornar

  guardar x en pila (-8(%rbp))
  guardar n en pila (-16(%rbp))

  %rdi = x * x          ← nueva base
  %rsi = n >> 1         ← n div 2 (shift aritmético)
  call potencia         ← resultado recursivo en %rax

  si n original era impar:
      %rax = %rax * x   ← multiplicar por x guardado

  epilogo, ret
```

### B. Sitio de llamada (`POW_OP` en `visit(BinaryExp)`)

Al llegar al caso `POW_OP`, el generador ya evaluó los operandos siguiendo el patrón estándar:
- `%rax` = base (`x`)
- `%rcx` = exponente (`n`)

```
rdi = rax          ← x como 1er argumento
rsi = rcx          ← n como 2do argumento
call potencia
// resultado en rax
```

---

## 3. Pseudocódigo Simplificado (Visitor)

```cpp
// En visit(Program) — emitir el helper una vez
void GenCodeVisitor::emitPotenciaHelper() {
    out << "potencia:\n";
    out << "  pushq %rbp\n";
    out << "  movq %rsp, %rbp\n";
    out << "  subq $16, %rsp\n";
    out << "  movq %rdi, -8(%rbp)\n";     // guardar x
    out << "  movq %rsi, -16(%rbp)\n";    // guardar n

    // caso base: n == 0
    out << "  cmpq $0, %rsi\n";
    out << "  jne potencia_check1\n";
    out << "  movq $1, %rax\n";
    out << "  jmp .end_potencia\n";

    // caso base: n == 1
    out << "potencia_check1:\n";
    out << "  cmpq $1, %rsi\n";
    out << "  jne potencia_check_par\n";
    out << "  movq %rdi, %rax\n";
    out << "  jmp .end_potencia\n";

    // verificar paridad: n & 1 == 0
    out << "potencia_check_par:\n";
    out << "  testq $1, %rsi\n";
    out << "  jne potencia_impar\n";

    // n par: potencia(x*x, n/2)
    out << "potencia_par:\n";
    out << "  movq -8(%rbp), %rdi\n";
    out << "  imulq %rdi, %rdi\n";        // x*x → %rdi
    out << "  movq -16(%rbp), %rsi\n";
    out << "  sarq $1, %rsi\n";           // n div 2
    out << "  call potencia\n";
    out << "  jmp .end_potencia\n";

    // n impar: potencia(x*x, n/2) * x
    out << "potencia_impar:\n";
    out << "  movq -8(%rbp), %rdi\n";
    out << "  imulq %rdi, %rdi\n";        // x*x → %rdi
    out << "  movq -16(%rbp), %rsi\n";
    out << "  sarq $1, %rsi\n";           // n div 2
    out << "  call potencia\n";
    out << "  imulq -8(%rbp), %rax\n";   // resultado * x original

    out << ".end_potencia:\n";
    out << "  leave\n";
    out << "  ret\n";
}

// En visit(BinaryExp) — caso POW_OP
case POW_OP:
    out << "  movq %rax, %rdi\n";   // base x
    out << "  movq %rcx, %rsi\n";   // exponente n
    out << "  call potencia\n";
    break;
```
