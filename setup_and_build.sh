#!/bin/bash

# Setup y Build Completo - Taller Parametrico GUI
# Uso: ./setup_and_build.sh

set -e

# Colores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  SETUP Y BUILD COMPLETO${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Verificar directorio
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}[ERROR] No se encontro CMakeLists.txt${NC}"
    exit 1
fi

# Buscar wxWidgets fuente
WX_SOURCE=""
if [ -d "../wxWidgets-3.3.2" ]; then
    WX_SOURCE="../wxWidgets-3.3.2"
elif [ -d "wxWidgets-3.3.2" ]; then
    WX_SOURCE="wxWidgets-3.3.2"
elif [ -d "/c/wxWidgets-3.3.2" ]; then
    WX_SOURCE="/c/wxWidgets-3.3.2"
fi

if [ -z "$WX_SOURCE" ]; then
    echo -e "${RED}[ERROR] No se encontro wxWidgets-3.3.2${NC}"
    echo "Descargar desde: https://github.com/wxWidgets/wxWidgets/releases/download/v3.3.2/wxWidgets-3.3.2.tar.bz2"
    echo "Extraer en: C:/wxWidgets-3.3.2"
    exit 1
fi

echo -e "${GREEN}[INFO]${NC} wxWidgets encontrado en: $WX_SOURCE"

# Crear directorio build/wxwidgets si no existe
if [ ! -d "build/wxwidgets" ]; then
    echo -e "${BLUE}[1/8]${NC} Creando directorio build/wxwidgets..."
    mkdir -p build/wxwidgets
    
    echo -e "${BLUE}[2/8]${NC} Configurando wxWidgets..."
    cd build/wxwidgets
    
    cmake "$SCRIPT_DIR/$WX_SOURCE" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$SCRIPT_DIR/build/wx_installed" \
        -DwxBUILD_SHARED=OFF \
        -DwxBUILD_SAMPLES=OFF \
        -DwxBUILD_DEMOS=OFF \
        -DwxBUILD_TESTS=OFF \
        -DwxUSE_STL=ON \
        -DwxUSE_OPENGL=OFF \
        -DwxUSE_HTML=OFF \
        -DwxUSE_MEDIACTRL=OFF \
        -DwxUSE_WEBVIEW=OFF \
        -DwxUSE_XRC=OFF \
        -DwxUSE_AUI=ON \
        -DwxUSE_PROPGRID=OFF \
        -DwxUSE_RIBBON=OFF \
        -DwxUSE_RICHTEXT=OFF \
        -DwxUSE_STC=OFF \
        -DCMAKE_C_COMPILER=/c/msys64/mingw64/bin/gcc.exe \
        -DCMAKE_CXX_COMPILER=/c/msys64/mingw64/bin/g++.exe
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERROR] Fallo la configuracion de wxWidgets${NC}"
        exit 1
    fi
    
    cd "$SCRIPT_DIR"
else
    echo -e "${BLUE}[1/8]${NC} Directorio build/wxwidgets ya existe"
fi

echo -e "${BLUE}[3/8]${NC} Compilando wxWidgets..."
echo "    (Esto tomara 20-40 minutos la primera vez)"
echo "    Compilando con $(nproc 2>/dev/null || echo 4) cores..."
echo ""
cd build/wxwidgets

ninja -j$(nproc 2>/dev/null || echo 4) 2>&1 | tee build.log | tail -20
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Fallo la compilacion de wxWidgets${NC}"
    echo "Ver build.log para detalles"
    exit 1
fi

echo ""
echo -e "${BLUE}[4/8]${NC} Instalando wxWidgets..."
ninja install
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Fallo la instalacion de wxWidgets${NC}"
    exit 1
fi

cd "$SCRIPT_DIR"

# Verificar instalacion
if [ ! -f "build/wx_installed/lib/wxWidgetsConfig.cmake" ]; then
    echo -e "${RED}[ERROR] wxWidgets no se instalo correctamente${NC}"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} wxWidgets instalado en build/wx_installed"

# Crear/limpiar build
echo -e "${BLUE}[5/8]${NC} Preparando build..."
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

if [ -f "CMakeCache.txt" ]; then
    echo -e "${BLUE}[6/8]${NC} Limpiando configuracion anterior..."
    rm -f CMakeCache.txt CMakeFiles -rf
fi

echo -e "${BLUE}[7/8]${NC} Configurando proyecto..."
cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DwxWidgets_DIR="$SCRIPT_DIR/build/wx_installed/lib"

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] CMake configuration failed${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[8/8]${NC} Compilando TallerParametrico..."
ninja TallerParametrico 2>&1 | tee build.log | tail -30

if [ $? -ne 0 ]; then
    echo ""
    echo -e "${YELLOW}[WARN]${NC} Intentando compilar dependencias primero..."
    ninja tp_shared tp_domain tp_simulation tp_persistence tp_application tp_presentation 2>&1 | tail -10
    ninja TallerParametrico 2>&1 | tail -20
fi

echo ""
echo -e "${BLUE}[9/9]${NC} Verificando..."

if [ -f "bin/TallerParametrico.exe" ]; then
    echo ""
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  [OK] TODO COMPILADO EXITOSAMENTE${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo -e "Ejecutable: bin/TallerParametrico.exe"
    ls -lh bin/TallerParametrico.exe
    echo ""
    
    read -p "Deseas ejecutar el programa ahora? (S/N): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Ss]$ ]]; then
        echo ""
        echo "Iniciando Taller Parametrico..."
        ./bin/TallerParametrico.exe
    fi
else
    echo -e "${RED}[ERROR] No se genero el ejecutable${NC}"
    echo "Archivos en bin/:"
    ls -la bin/*.exe 2>/dev/null || echo "No hay ejecutables"
    exit 1
fi

echo ""
echo -e "${GREEN}Listo!${NC}"
echo "Para volver a compilar solo: cd build && ninja TallerParametrico"
