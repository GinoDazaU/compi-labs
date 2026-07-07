# Datos curiosos sobre compilación

## Historia

### El primer compilador fue toda una apuesta
El primer compilador real fue el de FORTRAN (1957, equipo de IBM liderado por John Backus). En ese momento casi nadie creía que un programa pudiera generar código máquina tan bueno como el escrito a mano en ensamblador — tardaron años en convencer a la industria. Antes, Grace Hopper ya había construido el A-0 (1952), más un enlazador/traductor que un compilador completo.

### El término "bug" viene de un insecto de verdad
En 1947, Grace Hopper (la misma que años después escribió el A-0) y su equipo encontraron una polilla real atascada en un relé del Harvard Mark II, causando un mal funcionamiento. La pegaron en el cuaderno de bitácora con la nota "first actual case of bug being found" — de ahí quedó el término para errores de software.

### Por qué se llama "Dragon Book"
El libro clásico de compiladores (Aho, Sethi, Ullman — sí, el mismo Ullman de Sethi-Ullman) se conoce como el *Dragon Book* por la portada: un caballero luchando contra un dragón etiquetado "Complejidad de la Compilación". Es prácticamente el origen de cómo se enseña la estructura en fases: scanner → parser → semántico → codegen.

### JavaScript se diseñó en 10 días
Brendan Eich escribió el diseño inicial de JavaScript en 10 días en 1995, en Netscape. A diferencia de un lenguaje diseñado con teoría formal y gramática cuidada detrás (como las del proyecto del curso), muchas decisiones de JS quedaron "raras" (coerciones de tipo, `==` vs `===`) precisamente por esa prisa — un buen contraejemplo de por qué vale la pena diseñar la gramática y el chequeo de tipos con calma.

## Teoría

### Todo lenguaje Turing-completo es igual de poderoso
Por la tesis de Church-Turing, cualquier lenguaje que pueda simular una máquina de Turing (loops + condicionales + memoria) puede calcular exactamente lo mismo que cualquier otro. Un lenguaje nuevo no da más poder de cómputo — da mejor **abstracción**: más legible, más seguro, más cercano al problema. El compilador es el puente que traduce esa abstracción a las reglas rígidas de la máquina.

### Turing-completo no necesita casi nada de sintaxis
Lenguajes esotéricos como **Brainfuck** (8 símbolos en total) o **Whitespace** (cuyo código son solo espacios, tabs y saltos de línea) son Turing-completos: demuestran en la práctica que el "poder" de un lenguaje no depende de tener muchas construcciones, sino de tener lo mínimo — loops/condicionales + memoria.

### La jerarquía de Chomsky explica por qué el parser y el chequeo de tipos están separados
Un parser de gramática libre de contexto (BNF) puede reconocer estructura anidada (paréntesis balanceados, bloques), pero **no puede** expresar reglas como "esta variable fue declarada antes de usarse" — eso requiere memoria/estado arbitrario, que ya no es libre de contexto. Por eso siempre hay una fase aparte con entorno (chequeo semántico) después del parser.

### Por qué existen las optimizaciones (y por qué no son "perfectas")
Decidir si una optimización siempre es válida (ej. "¿esta llamada de función siempre puede reemplazarse por su resultado constante?", "¿este código es alcanzable?") es, en el caso general, equivalente al **problema de la parada** (halting problem) — indecidible. Por eso los compiladores reales usan heurísticas conservadoras (como la regla de "cascada": solo se aplica si se puede probar estáticamente que es segura) en vez de intentar resolverlo perfectamente.

### Sethi-Ullman no es una heurística cualquiera
El algoritmo que etiqueta nodos para decidir qué lado evaluar primero (1975, Sethi y Ullman) es **óptimo**: está demostrado matemáticamente que minimiza la cantidad de registros necesarios para evaluar una expresión aritmética, dado un árbol de expresión y un número fijo de registros.

### El teorema de Rice: la indecidibilidad no es solo sobre "si el programa termina"
El problema de la parada es un caso particular de algo más general: el **teorema de Rice** dice que *cualquier* propiedad no trivial sobre el comportamiento de un programa (¿esta variable siempre vale lo mismo?, ¿esta función siempre retorna positivo?, ¿este código es realmente inalcanzable?) es indecidible en general. Por eso ningún compilador puede detectar *todas* las oportunidades de optimización posibles — solo las que caen dentro de un patrón que decidió analizar (plegado, cascada, mirilla, etc.).

### Cada nivel de la jerarquía de Chomsky tiene su propia "máquina"
Hay una correspondencia exacta: lenguajes regulares ↔ autómatas finitos (el scanner/lexer), lenguajes libres de contexto ↔ autómatas de pila (el parser), lenguajes dependientes del contexto e irrestrictos ↔ máquinas de Turing (donde caen las reglas semánticas tipo "variable declarada antes de usarse"). La arquitectura en fases de un compilador (scanner → parser → semántico) refleja literalmente subir un escalón en esta jerarquía en cada fase.

### Asignar registros de forma óptima es NP-completo
Cuando hay más variables vivas simultáneamente que registros físicos disponibles, decidir la asignación óptima de registros equivale a colorear un grafo con el mínimo de colores (*graph coloring*), un problema NP-completo. Por eso los compiladores de producción (GCC, LLVM) no buscan la solución óptima, usan heurísticas de *graph coloring* con "spilling" a memoria cuando no alcanzan los registros.

### Existen compiladores matemáticamente probados como correctos
CompCert es un compilador de C cuyo backend está demostrado formalmente (en el asistente de pruebas Coq) para nunca introducir un bug de compilación: se probó matemáticamente que el código generado siempre preserva el significado del código fuente. Es la excepción — casi ningún compilador comercial (GCC, Clang) tiene esa garantía, solo tests extensivos.

### El "Full Employment Theorem for Compiler Writers"
Un resultado medio en broma, medio serio: nunca puede existir un compilador que genere siempre el código objetivamente más pequeño o más rápido posible para cualquier programa — es corolario directo del teorema de Rice. En la práctica significa que siempre habrá margen (y trabajo) para seguir mejorando optimizadores, nunca se "resuelve" del todo.

### La interpretación abstracta unifica casi todo lo que ya viste
Plegado de constantes, chequeo de tipos y detección de código muerto son, técnicamente, instancias del mismo marco teórico general: la **interpretación abstracta** (Cousot & Cousot, 1977), que consiste en "ejecutar" el programa sobre una versión aproximada de sus valores (constante conocida / tipo / alcanzable-o-no) en vez de sus valores reales en tiempo de ejecución.

### El teorema de Kleene: regex, DFA y NFA son lo mismo
Está demostrado que toda expresión regular, todo autómata finito determinista (DFA) y todo autómata finito no determinista (NFA) reconocen exactamente la misma familia de lenguajes. Por eso el scanner de un compilador se puede especificar con expresiones regulares y compilarlas mecánicamente a un DFA — no es una elección de conveniencia, es una equivalencia matemática garantizada.

### El lema del bombeo explica por qué el scanner no basta
El *pumping lemma* prueba formalmente que ningún autómata finito (ni regex) puede reconocer un lenguaje como "paréntesis balanceados" — necesitaría memoria ilimitada para contar cuántos abrió, y un DFA solo tiene estados finitos. Esa es la razón matemática, no solo práctica, de por qué hace falta una gramática libre de contexto y una pila (el parser) para estructuras anidadas.

### Los compiladores modernos no trabajan sobre el AST directamente
LLVM y GCC transforman el AST a una representación intermedia llamada **SSA (Static Single Assignment)**, donde cada variable se asigna exactamente una vez. Eso simplifica enormemente muchos análisis de optimización (saber de dónde viene cada valor es trivial cuando no hay reasignaciones), a costa de tener que reconstruir el código real al final.

### El comportamiento indefinido (UB) es una licencia para optimizar, no un error de diseño
En C/C++, cosas como desbordar un entero con signo o desreferenciar un puntero nulo son "comportamiento indefinido": el compilador asume por contrato que el programador nunca hace eso, y usa esa suposición para optimizar agresivamente (eliminar chequeos, reordenar código). Es una consecuencia práctica y polémica del teorema de Rice — el compilador no puede demostrar que el UB nunca ocurre, así que simplemente asume que no ocurre.

## Práctica / ingeniería

### El peephole (mirilla) es la optimización más "artesanal"
A diferencia de plegado de constantes o Sethi-Ullman (que tienen una teoría formal detrás), la optimización por mirilla es literalmente una lista de patrones observados empíricamente en el ensamblador generado — no hay una prueba matemática de que sea óptima, solo que funciona bien en la práctica. Los compiladores reales (GCC, LLVM) tienen miles de estos patrones acumulados.

### Un mismo front-end puede alimentar muchos back-ends (y viceversa)
Esa es la idea detrás de LLVM: cualquier lenguaje que genere su representación intermedia (LLVM IR) obtiene gratis los optimizadores y generadores de código para x86, ARM, RISC-V, etc. Es la razón por la que Rust, Swift, Clang y Julia comparten la misma infraestructura de optimización aunque sus sintaxis no tengan nada que ver.

### Bootstrapping: un compilador que se compila a sí mismo
Muchos compiladores (GCC, el compilador de Go, rustc) están escritos en el mismo lenguaje que compilan. La primera versión se compila con otro compilador ya existente; las siguientes versiones se compilan consigo mismas. A esto se le llama *bootstrapping*.

### Interpretar vs compilar no es una dicotomía real
En la práctica casi todo es híbrido: Python compila a bytecode antes de interpretarlo; Java compila a bytecode y luego un JIT (just-in-time) lo compila a código máquina *mientras corre*, usando información de perfilado en tiempo real que un compilador estático nunca tiene.

### La industria todavía confía en compiladores "viejos"
NASA y bancos siguen usando FORTRAN y COBOL (de los años 50-60) en sistemas críticos, no por nostalgia sino porque esos compiladores llevan décadas probados y su comportamiento es predecible — la estabilidad de un compilador maduro a veces pesa más que usar el lenguaje más nuevo.

### No todos los errores se reportan y se detienen igual
Un compilador de producción no se detiene en el primer error semántico — intenta seguir analizando el resto del archivo para reportar *todos* los errores de una vez (recuperación de errores), usando heurísticas para no generar una cascada de errores falsos derivados del primero.

## Preguntas para hacerle al profe

- Si todo lenguaje Turing-completo puede calcular lo mismo, ¿para qué seguimos creando lenguajes nuevos? ¿Es solo por abstracción/legibilidad, o hay algo más?
- ¿Existe algún límite teórico (tipo indecidibilidad) que haga que un compilador jamás pueda optimizar "perfecto", por más avanzado que sea?
- ¿Por qué la mirilla no tiene una teoría formal detrás como Sethi-Ullman? ¿Alguien lo ha intentado formalizar?
- Con JIT (compilación en caliente, como Java o V8), ¿tiene sentido seguir enseñando compilación estática, o el futuro es todo híbrido?
- ¿Un compilador con IA (que "aprenda" patrones de optimización en vez de reglas fijas) podría superar algún día a GCC/LLVM?
- ¿Por qué el chequeo semántico no se puede meter dentro de la gramática (BNF) y siempre necesita una pasada aparte?
