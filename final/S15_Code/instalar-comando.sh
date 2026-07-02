#!/usr/bin/env bash
set -euo pipefail

PROJECT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_DIR="$HOME/.local/bin"
COMMAND_PATH="$BIN_DIR/GonzaloGCC"

mkdir -p "$BIN_DIR"

g++ -std=c++17 -O2 -o "$COMMAND_PATH" \
    "$PROJECT_PATH/main.cpp" \
    "$PROJECT_PATH/scanner.cpp" \
    "$PROJECT_PATH/token.cpp" \
    "$PROJECT_PATH/parser.cpp" \
    "$PROJECT_PATH/ast.cpp" \
    "$PROJECT_PATH/visitor.cpp"

chmod +x "$COMMAND_PATH"

case ":$PATH:" in
    *":$BIN_DIR:"*)
        echo "Listo. Ya puedes usar: compilador <archivo_de_entrada>"
        ;;
    *)
        echo "Listo. Se instalo el comando en $COMMAND_PATH"
        echo "Agrega esta linea a ~/.bashrc o ~/.zshrc si tu terminal no lo encuentra:"
        echo "export PATH=\"\$HOME/.local/bin:\$PATH\""
        ;;
esac
