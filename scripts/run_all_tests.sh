#!/bin/bash

# Ejecuta toda la suite de tests del proyecto.

set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "[ERROR] No existe el directorio build/. Compila primero el proyecto."
    exit 1
fi

cd "$BUILD_DIR"

echo "========================================"
echo "EJECUTANDO TODOS LOS TESTS"
echo "========================================"
echo

ctest --output-on-failure
