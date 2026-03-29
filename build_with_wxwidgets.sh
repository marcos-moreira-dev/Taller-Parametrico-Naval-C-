#!/bin/bash

# Build con wxWidgets - Taller Parametrico GUI
# Uso: ./build_with_wxwidgets.sh

set -e

# Colores
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  BUILD CON WXWIDGETS - Taller Parametrico${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Verificar directorio
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}[ERROR] No se encontro CMakeLists.txt${NC}"
    exit 1
fi

# Verificar wxwidgets
if [ ! -d "build/wxwidgets" ]; then
    echo -e "${RED}[ERROR] No existe build/wxwidgets${NC}"
    echo "Ejecuta primero: ./setup_wxwidgets.sh"
    exit 1
fi

echo -e "${BLUE}[1/6]${NC} Compilando wxWidgets..."
echo "    (Esto tomara 15-30 minutos la primera vez)"
echo ""
cd build/wxwidgets
cmake --build . --parallel $(nproc 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Fallo la compilacion de wxWidgets${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[2/6]${NC} Instalando wxWidgets..."
cmake --install .
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] Fallo la instalacion de wxWidgets${NC}"
    exit 1
fi

cd "$SCRIPT_DIR"

if [ ! -d "build" ]; then
    mkdir build
fi

cd build

echo ""
echo -e "${BLUE}[3/6]${NC} Limpiando build anterior..."
rm -rf CMakeCache.txt CMakeFiles

echo ""
echo -e "${BLUE}[4/6]${NC} Configurando CMake..."
cmake .. -G Ninja
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] CMake configuration failed${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[5/6]${NC} Compilando TallerParametrico..."
ninja TallerParametrico
if [ $? -ne 0 ]; then
    echo ""
    echo -e "${YELLOW}[WARN]${NC} Intentando compilar dependencias primero..."
    ninja tp_shared tp_domain tp_simulation tp_persistence tp_application tp_presentation
    ninja TallerParametrico
fi

echo ""
echo -e "${BLUE}[6/6]${NC} Verificando..."

if [ -f "bin/TallerParametrico.exe" ]; then
    echo ""
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  [OK] COMPILACION EXITOSA - GUI${NC}"
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
    ls -la bin/*.exe 2>/dev/null || echo "No hay ejecutables"
    exit 1
fi

echo ""
echo -e "${GREEN}Listo!${NC}"
