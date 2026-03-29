#!/bin/bash
# Script completo para compilar wxWidgets y el proyecto

set -e

echo "========================================"
echo "Compilacion completa con wxWidgets"
echo "========================================"
echo ""

WX_DIR="/c/Users/MARCOS MOREIRA/Downloads/wxWidgets-3.3.2"
BUILD_DIR="/c/Users/MARCOS MOREIRA/Downloads/cplus/build"

echo "Paso 1: Configurando wxWidgets..."
mkdir -p "$BUILD_DIR/wxwidgets"
cd "$BUILD_DIR/wxwidgets"
cmake "$WX_DIR" -G "Ninja" \
    -DCMAKE_BUILD_TYPE=Release \
    -DwxBUILD_SHARED=OFF \
    -DwxUSE_WEBVIEW=OFF \
    -DwxUSE_MEDIACTRL=OFF \
    -DCMAKE_INSTALL_PREFIX="$BUILD_DIR/../wx_installed"

echo ""
echo "Paso 2: Compilando wxWidgets (aprox 10-15 minutos)..."
cmake --build . --parallel 2

echo ""
echo "Paso 3: Configurando proyecto..."
cd "$BUILD_DIR"
cmake .. -G "Ninja" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    -DBUILD_GUI=ON \
    -DwxWidgets_ROOT_DIR="$BUILD_DIR/../wx_installed"

echo ""
echo "Paso 4: Compilando proyecto..."
cmake --build . --parallel 2

echo ""
echo "========================================"
echo "COMPILACION COMPLETADA!"
echo "========================================"
echo "Ejecutable: $BUILD_DIR/bin/TallerParametrico.exe"
echo ""
