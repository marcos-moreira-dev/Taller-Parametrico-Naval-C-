#!/bin/bash

# Build Automático - Taller Parametrico
# Uso: ./build_all.sh [clean]

set -e  # Exit on error

# Colores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}============================================${NC}"
echo -e "${BLUE}  BUILD AUTOMATICO - Taller Parametrico${NC}"
echo -e "${BLUE}============================================${NC}"
echo ""

# Detectar directorio del script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Verificar CMakeLists.txt
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}[ERROR] No se encontro CMakeLists.txt${NC}"
    echo "Asegurate de ejecutar este script desde la carpeta del proyecto"
    exit 1
fi

# Crear directorio build si no existe
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Limpiar si se solicita
if [ "$1" == "clean" ]; then
    echo -e "${YELLOW}[INFO] Limpiando build...${NC}"
    rm -rf *
fi

echo -e "${BLUE}[1/4]${NC} Configurando CMake..."
cmake .. -G Ninja
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] CMake configuration failed${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[2/4]${NC} Verificando targets disponibles..."

# Detectar qué target usar
if ninja -t targets 2>/dev/null | grep -q "TallerParametrico.exe"; then
    TARGET="TallerParametrico"
    echo -e "${GREEN}[INFO]${NC} Compilando TallerParametrico (GUI)..."
elif ninja -t targets 2>/dev/null | grep -q "TallerParametrico_Console.exe"; then
    TARGET="TallerParametrico_Console"
    echo -e "${YELLOW}[INFO]${NC} wxWidgets no disponible, compilando version consola..."
else
    echo -e "${RED}[ERROR]${NC} No se encontro ningun target valido"
    echo "Targets disponibles:"
    ninja -t targets | grep -i "taller\|tp_" || true
    exit 1
fi

echo ""
echo -e "${BLUE}[3/4]${NC} Compilando... (esto puede tomar unos minutos)"
ninja "$TARGET"
if [ $? -ne 0 ]; then
    echo ""
    echo -e "${YELLOW}[WARN]${NC} Compilacion inicial fallida, intentando con dependencias..."
    ninja tp_shared tp_domain tp_simulation tp_persistence tp_application
    ninja "$TARGET"
fi

echo ""
echo -e "${BLUE}[4/4]${NC} Verificando ejecutable..."

if [ -f "bin/${TARGET}.exe" ]; then
    echo ""
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  [OK] COMPILACION EXITOSA${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo -e "Ejecutable: bin/${TARGET}.exe"
    echo ""
    
    # Preguntar si ejecutar
    read -p "Deseas ejecutar el programa ahora? (S/N): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Ss]$ ]]; then
        echo ""
        echo "Iniciando..."
        ./bin/${TARGET}.exe
    fi
else
    echo ""
    echo -e "${RED}[ERROR] No se genero el ejecutable${NC}"
    echo "Archivos en bin/:"
    ls -la bin/*.exe 2>/dev/null || echo "(directorio vacio)"
    exit 1
fi

echo ""
echo -e "${GREEN}Listo!${NC}"
