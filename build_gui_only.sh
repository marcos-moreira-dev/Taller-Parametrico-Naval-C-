#!/bin/bash

# Build GUI - Script simple y directo para compilar solo la version GUI
# Uso: ./build_gui_only.sh

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  BUILD GUI - Taller Parametrico${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

cd "$(dirname "$0")"

# PASO 1: Verificar wxWidgets instalado
echo -e "${BLUE}[1/3]${NC} Verificando wxWidgets..."

if [ ! -d "wx_installed" ]; then
    echo -e "${RED}[ERROR] No existe wx_installed${NC}"
    echo "Debes compilar wxWidgets primero con build_complete.sh"
    exit 1
fi

if [ ! -f "wx_installed/include/wx/wx.h" ]; then
    echo -e "${RED}[ERROR] wxWidgets incompleto${NC}"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} wxWidgets encontrado"

# PASO 2: Limpiar y preparar build
echo ""
echo -e "${BLUE}[2/3]${NC} Preparando build..."

if [ -d "build" ]; then
    echo "Limpiando build anterior..."
    rm -rf build
fi

mkdir -p build
cd build

# PASO 3: Configurar CMake
echo ""
echo -e "${BLUE}[3/3]${NC} Configurando CMake..."

cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
    -DCMAKE_CXX_COMPILER=/c/msys64/mingw64/bin/g++.exe \
    -DWX_INSTALL_DIR=../wx_installed \
    -DBUILD_GUI=ON

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] CMake configuration failed${NC}"
    echo "Intentando sin especificar compiladores..."
    
    cmake .. -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DWX_INSTALL_DIR=../wx_installed \
        -DBUILD_GUI=ON
fi

# PASO 4: Compilar
echo ""
echo -e "${BLUE}[4/4]${NC} Compilando..."

ninja TallerParametrico 2>&1 | tail -30

# Verificar resultado
echo ""
echo -e "${BLUE}Verificando...${NC}"

if [ -f "bin/TallerParametrico.exe" ]; then
    echo ""
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  [OK] GUI COMPILADA EXITOSAMENTE${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
    ls -lh bin/TallerParametrico.exe
    
    # Copiar DLLs
    cp /c/msys64/mingw64/bin/libwinpthread-1.dll bin/ 2>/dev/null || true
    cp /c/msys64/mingw64/bin/libgcc_s_seh-1.dll bin/ 2>/dev/null || true  
    cp /c/msys64/mingw64/bin/libstdc++-6.dll bin/ 2>/dev/null || true
    
    echo ""
    echo "Para ejecutar:"
    echo "  cd build/bin"
    echo "  ./TallerParametrico.exe"
    echo ""
    read -p "Ejecutar ahora? (S/N): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Ss]$ ]]; then
        cd bin
        ./TallerParametrico.exe
    fi
else
    echo -e "${RED}[ERROR] No se genero el ejecutable${NC}"
    echo "Contenido de bin/:"
    ls -la bin/ 2>/dev/null || echo "(vacio)"
    exit 1
fi

echo ""
echo -e "${GREEN}Listo!${NC}"
