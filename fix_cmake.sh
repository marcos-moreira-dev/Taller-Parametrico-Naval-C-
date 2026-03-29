#!/bin/bash

# Fix CMake - Configura CMake para usar rutas Unix correctamente

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  FIX CMAKE CONFIGURATION${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

cd "$(dirname "$0")"

# Verificar compilador
echo -e "${BLUE}[1/4]${NC} Verificando compilador..."

C_COMPILER="/c/msys64/mingw64/bin/gcc.exe"
CXX_COMPILER="/c/msys64/mingw64/bin/g++.exe"

if [ ! -f "$C_COMPILER" ]; then
    echo -e "${RED}[ERROR] No se encontro gcc.exe${NC}"
    exit 1
fi

if [ ! -f "$CXX_COMPILER" ]; then
    echo -e "${RED}[ERROR] No se encontro g++.exe${NC}"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} Compiladores encontrados"
echo "  C: $C_COMPILER"
echo "  C++: $CXX_COMPILER"

# Verificar wxWidgets
echo ""
echo -e "${BLUE}[2/4]${NC} Verificando wxWidgets..."

WX_DIR="$PWD/wx_installed"
if [ ! -d "$WX_DIR" ]; then
    echo -e "${RED}[ERROR] wxWidgets no encontrado en $WX_DIR${NC}"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} wxWidgets en: $WX_DIR"

# Preparar directorio build
echo ""
echo -e "${BLUE}[3/4]${NC} Preparando build..."

BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}[WARN]${NC} Limpiando build existente..."
    # Intentar limpiar archivos CMake sin borrar el directorio
    rm -f "$BUILD_DIR/CMakeCache.txt" 2>/dev/null || true
    rm -rf "$BUILD_DIR/CMakeFiles" 2>/dev/null || true
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar CMake con rutas Unix explícitas
echo ""
echo -e "${BLUE}[4/4]${NC} Configurando CMake..."
echo ""

# Exportar variables para CMake
export CC="$C_COMPILER"
export CXX="$CXX_COMPILER"

# Configurar con rutas absolutas en formato Unix
cmake .. \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER="$C_COMPILER" \
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
    -DWX_INSTALL_DIR="$WX_DIR" \
    -DBUILD_GUI=ON \
    -DCMAKE_MAKE_PROGRAM=/c/msys64/mingw64/bin/mingw32-make.exe \
    2>&1

if [ $? -ne 0 ]; then
    echo ""
    echo -e "${YELLOW}[INFO]${NC} Intentando con Ninja (formato alternativo)..."
    
    # Limpiar y reintentar
    rm -f CMakeCache.txt
    rm -rf CMakeFiles
    
    cmake .. \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER="$C_COMPILER" \
        -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
        -DWX_INSTALL_DIR="$WX_DIR" \
        -DBUILD_GUI=ON \
        -DCMAKE_C_COMPILER_WORKS=1 \
        -DCMAKE_CXX_COMPILER_WORKS=1 \
        2>&1
fi

echo ""
echo -e "${GREEN}============================================${NC}"
echo -e "${GREEN}  [OK] CMAKE CONFIGURADO${NC}"
echo -e "${GREEN}============================================${NC}"
echo ""
echo "Para compilar ahora ejecuta:"
echo "  cd build"
echo "  ninja TallerParametrico"
echo ""
