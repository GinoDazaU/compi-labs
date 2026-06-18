# Implementación de Matrices Rectangulares en el Compilador

Este documento explica a alto nivel la lógica para dar soporte a **matrices rectangulares** dinámicas sobre el heap utilizando una representación plana unidimensional.

## 1. Idea de Alto Nivel (Representación Plana)
En lugar de estructurar la matriz como un arreglo de arreglos (puntero a puntero), mapeamos la matriz bidimensional de dimensiones $R \times C$ a un único bloque continuo de memoria de tamaño $R \times C \times 8$ bytes.

$$M[i][j] \implies \text{Arreglo}[i \times C + j]$$

### Ventajas:
*   **Eficiencia:** Requiere únicamente una llamada a `malloc` durante la creación.
*   **Simplicidad:** No requiere bucles en el código ensamblador generado para inicializar punteros de fila.
*   **Acceso Directo:** La aritmética de direccionamiento se resuelve en tiempo de ejecución combinando multiplicación y suma.

---

## 2. Flujo de Operaciones y Aritmética x86-64

### A. Creación (`MatrixDecl`)
Calculamos en tiempo de compilación el total de elementos ($R \times C$). Multiplicamos por 8 bytes y solicitamos memoria en el heap. El puntero retornado se almacena en la pila local.

```
totalElements = rows * cols
bytes = totalElements * 8
puntero = malloc(bytes)
pila[nombre] = puntero
```

### B. Acceso a Elemento (`MatrixAccessExp`)
Para acceder a `matrix[rowExpr][colExpr]`, realizamos la conversión del índice a una dimensión lineal $k$:

1.  Evaluamos `rowExpr` en `%rax`.
2.  Multiplicamos por el número de columnas (`Cols` de la matriz) usando `imulq $Cols, %rax`.
3.  Evaluamos `colExpr` en `%rax` y lo sumamos a la multiplicación previa.
4.  Cargamos la base de la matriz y accedemos a la celda usando direccionamiento indexado escalado en 8 bytes: `(%base, %rax, 8)`.

```
rax = rowExpr
rax = rax * Cols
rcx = colExpr
rax = rax + rcx
base = pila[nombre]
rax = * (base + rax * 8)
```

### C. Asignación (`MatrixAssignStm`)
Para escribir `matrix[rowExpr][colExpr] = valueExpr`:
1.  Calculamos el índice lineal de destino ($k = rowExpr \times Cols + colExpr$) y lo guardamos temporalmente.
2.  Evaluamos la expresión de valor y lo guardamos en `%rcx`.
3.  Cargamos la dirección base del heap en `%rax`.
4.  Escribimos en memoria el valor usando el índice destino: `*(rax + index * 8) = rcx`.

---

## 3. Ejemplo de Pseudocódigo Simplificado (Visitor)

```cpp
// Declaración (Ej: matrix[3][4])
void visit(MatrixDecl* stm) {
    int totalElements = stm->rows * stm->cols;
    emit("movq $" + totalElements + ", %rax");
    emit("imulq $8, %rax");
    emit("movq %rax, %rdi");
    emit("call malloc@PLT");
    emit("movq %rax, offset(%rbp)"); // Guardar en slot de pila
}

// Lectura (Ej: matrix[r][c])
void visit(MatrixAccessExp* exp) {
    exp->rowExpr->accept(this);      // %rax = r
    emit("imulq $Cols, %rax");       // %rax = r * Cols
    emit("pushq %rax");
    exp->colExpr->accept(this);      // %rax = c
    emit("popq %rcx");
    emit("addq %rcx, %rax");         // %rax = r * Cols + c
    emit("movq base(%rbp), %rcx");   // %rcx = puntero heap
    emit("movq (%rcx, %rax, 8), %rax"); // %rax = M[r * Cols + c]
}

// Escritura (Ej: matrix[r][c] = valor)
void visit(MatrixAssignStm* stm) {
    stm->rowExpr->accept(this);      // %rax = r
    emit("imulq $Cols, %rax");       // %rax = r * Cols
    emit("pushq %rax");
    stm->colExpr->accept(this);      // %rax = c
    emit("popq %rcx");
    emit("addq %rcx, %rax");         // %rax = r * Cols + c (índice k)
    emit("pushq %rax");
    stm->valueExpr->accept(this);    // %rax = valor a asignar
    emit("movq %rax, %rcx");         // %rcx = valor
    emit("popq %rdi");               // %rdi = índice k
    emit("movq base(%rbp), %rax");   // %rax = puntero heap
}
```

