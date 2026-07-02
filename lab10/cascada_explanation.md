# Optimización en Cascada dentro del Compilador

La optimización en cascada combina el plegado de constantes y el etiquetado Sethi-Ullman de forma encadenada. El plegado marca los nodos como constantes (`isConstant`, `constantValue`); Sethi-Ullman aprovecha esa información asignando `label = 0` a los nodos ya plegados. GenCode usa ambas anotaciones: si un nodo es constante lo carga directamente, y si no, aplica el orden de evaluación indicado por el label.

> La cascada **depende** de que las dos fases anteriores hayan corrido primero sobre el AST.

---

## 1. Idea de Alto Nivel

Los atributos relevantes en `Exp`:

```
isConstant    → true si toda la subexpresión se puede evaluar en compilación
constantValue → valor calculado en compilación
label         → mínimo de registros necesarios (Sethi-Ullman)
```

El efecto cascada ocurre en dos momentos:

1. **SethiUllman**: si `isConstant == true`, asignar `label = 0` en lugar de calcular normalmente.
2. **GenCode**: si `isConstant == true`, emitir `movq $val` directamente (corto-circuito total).

$$\text{label} = 0 \implies \text{no ocupa registro de la pila}$$

---

## 2. Flujo de Operaciones

### A. Sethi-Ullman con cascada (`SethiUllmanVisitor`)

```
si exp->isConstant:
    exp->label = 0          ← cascada: nodo colapsado, no cuesta registro
    return

NumberExp / IdExp:
    exp->label = 1

BinaryExp (no constante):
    l = left->label
    r = right->label
    si l == r: exp->label = l + 1
    si no:     exp->label = max(l, r)
```

### B. Generación de código con cascada (`GenCodeVisitor::visit(BinaryExp)`)

```
si exp->isConstant:
    emit("movq $" + constantValue + ", %rax")
    return                              ← corto-circuito total

l = left->label
r = right->label

si r == 0:                              ← cascada: derecho es constante, sin push
    eval left  → %rax
    emit("movq %rax, %rcx")
    eval right → %rax                   ← emite solo "movq $val, %rax"
    emit("xchgq %rax, %rcx")           ← %rax=izq, %rcx=der

si l >= r:
    eval left  → %rax
    emit("pushq %rax")
    eval right → %rax
    emit("movq %rax, %rcx")
    emit("popq %rax")

si l < r:
    eval right → %rax
    emit("pushq %rax")
    eval left  → %rax
    emit("movq %rax, %rcx")
    emit("popq %rax")
    emit("xchgq %rax, %rcx")

// %rax = izquierdo, %rcx = derecho
emit(operacion)
```

---

## 3. Pseudocódigo Simplificado (Visitor)

```cpp
// SethiUllman con cascada
int SethiUllmanVisitor::visit(BinaryExp* exp) {
    if (exp->isConstant) { exp->label = 0; return 0; }  // cascada
    exp->left->accept(this);
    exp->right->accept(this);
    int l = exp->left->label, r = exp->right->label;
    exp->label = (l == r) ? l + 1 : max(l, r);
    return 0;
}

int SethiUllmanVisitor::visit(NumberExp* exp) {
    exp->label = exp->isConstant ? 0 : 1;
    return 0;
}

int SethiUllmanVisitor::visit(IdExp* exp) {
    exp->label = 1;
    return 0;
}

// GenCode con cascada
int GenCodeVisitor::visit(BinaryExp* exp) {
    if (exp->isConstant) {
        out << "  movq $" << exp->constantValue << ", %rax\n";
        return 0;
    }

    int l = exp->left->label, r = exp->right->label;

    if (r == 0) {
        exp->left->accept(this);
        out << "  movq %rax, %rcx\n";
        exp->right->accept(this);          // solo emite movq $val, %rax
        out << "  xchgq %rax, %rcx\n";
    } else if (l >= r) {
        exp->left->accept(this);
        out << "  pushq %rax\n";
        exp->right->accept(this);
        out << "  movq %rax, %rcx\n";
        out << "  popq %rax\n";
    } else {
        exp->right->accept(this);
        out << "  pushq %rax\n";
        exp->left->accept(this);
        out << "  movq %rax, %rcx\n";
        out << "  popq %rax\n";
        out << "  xchgq %rax, %rcx\n";
    }

    switch (exp->op) {
        case PLUS_OP:  out << "  addq  %rcx, %rax\n"; break;
        case MINUS_OP: out << "  subq  %rcx, %rax\n"; break;
        case MUL_OP:   out << "  imulq %rcx, %rax\n"; break;
        case DIV_OP:   out << "  cqto\n";
                       out << "  idivq %rcx\n";        break;
    }
    return 0;
}
```
