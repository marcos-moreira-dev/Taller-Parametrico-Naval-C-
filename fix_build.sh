#!/bin/bash

# Fix Build - Instala wxWidgets y copia DLLs necesarios

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  ARREGLANDO BUILD${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

cd "$(dirname "$0")"

echo -e "${BLUE}[1/5]${NC} Instalando wxWidgets..."
cd build/wxwidgets
ninja install
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Fallo instalacion de wxWidgets${NC}"
    exit 1
fi
cd ../..

echo -e "${GREEN}[OK]${NC} wxWidgets instalado en build/wx_installed"
echo ""

echo -e "${BLUE}[2/5]${NC} Copiando DLLs necesarios..."

# Crear directorio bin si no existe
mkdir -p build/bin

# Copiar DLLs de MSYS2/MinGW
DLLS=(
    "/c/msys64/mingw64/bin/libwinpthread-1.dll"
    "/c/msys64/mingw64/bin/libgcc_s_seh-1.dll"
    "/c/msys64/mingw64/bin/libstdc++-6.dll"
)

for dll in "${DLLS[@]}"; do
    if [ -f "$dll" ]; then
        cp "$dll" build/bin/
        echo "  Copiado: $(basename $dll)"
    else
        echo -e "${YELLOW}[WARN]${NC} No encontrado: $dll"
    fi
done

echo ""
echo -e "${BLUE}[3/5]${NC} Limpiando configuracion anterior..."
cd build
rm -f CMakeCache.txt CMakeFiles -rf
cd ..

echo ""
echo -e "${BLUE}[4/5]${NC} Reconfigurando con wxWidgets instalado..."
cd build

cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DwxWidgets_DIR="$PWD/wx_installed/lib"

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] CMake configuration failed${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[5/5]${NC} Compilando TallerParametrico (GUI)..."
ninja TallerParametrico 2>&1 | tail -20

if [ $? -ne 0 ]; then
    echo -e "${YELLOW}[WARN]${NC} Intentando con dependencias..."
    ninja tp_shared tp_domain tp_simulation tp_persistence tp_application tp_presentation
    ninja TallerParametrico
fi

echo ""
echo -e "${BLUE}Verificando...${NC}"

if [ -f "bin/TallerParametrico.exe" ]; then
    echo ""
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  [OK] BUILD ARREGLADO${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
    echo "Ejecutables generados:"
    ls -lh bin/*.exe 2>/dev/null || echo "No hay ejecutables"
    echo ""
    echo "DLLs incluidos:"
    ls -lh bin/*.dll 2>/dev/null || echo "No hay DLLs"
    echo ""
    echo -e "${GREEN}Para ejecutar:${NC}"
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
    echo -e "${RED}[ERROR] No se genero TallerParametrico.exe${NC}"
    ls -la bin/ 2>/dev/null || echo "No hay bin/"
    exit 1
fi

echo ""
echo -e "${GREEN}Listo!${NC}"
