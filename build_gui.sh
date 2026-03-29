#!/bin/bash

# Build GUI - Con manejo de directorio build bloqueado

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

# Verificar wxWidgets
echo -e "${BLUE}[1/3]${NC} Verificando wxWidgets..."

if [ ! -d "wx_installed" ] || [ ! -f "wx_installed/include/wx/wx.h" ]; then
    echo -e "${RED}[ERROR] wxWidgets no instalado${NC}"
    echo "Ejecuta primero: ./build_complete.sh"
    exit 1
fi

echo -e "${GREEN}[OK]${NC} wxWidgets encontrado"

# Usar build2 si build esta bloqueado
BUILD_DIR="build"
if [ -d "build" ]; then
    echo -e "${YELLOW}[WARN]${NC} Directorio 'build' bloqueado, usando 'build2'..."
    BUILD_DIR="build2"
fi

# PASO 2: Limpiar build
echo ""
echo -e "${BLUE}[2/3]${NC} Preparando $BUILD_DIR..."

if [ -d "$BUILD_DIR" ]; then
    echo "Limpiando $BUILD_DIR..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# PASO 3: Configurar CMake
echo ""
echo -e "${BLUE}[3/3]${NC} Configurando CMake..."

# Configurar con wxWidgets
cmake .. -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DWX_INSTALL_DIR=../wx_installed \
    -DBUILD_GUI=ON \
    2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}[ERROR] CMake configuration failed${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}[4/4]${NC} Compilando GUI..."

# Compilar solo el target GUI
ninja TallerParametrico 2>&1 | tail -30

# Verificar
echo ""
echo -e "${BLUE}Verificando resultado...${NC}"

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
    echo -e "${GREEN}Para ejecutar:${NC}"
    echo "  cd $BUILD_DIR/bin"
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
    ls -la bin/ 2>/dev/null || echo "bin/ no existe"
    
    # Intentar compilar dependencias
    echo -e "${YELLOW}[INFO]${NC} Intentando compilar dependencias..."
    ninja tp_shared tp_domain tp_simulation tp_persistence tp_application tp_presentation
    ninja TallerParametrico
    
    if [ -f "bin/TallerParametrico.exe" ]; then
        echo -e "${GREEN}[OK]${NC} Exito en segundo intento!"
    else
        exit 1
    fi
fi

echo ""
echo -e "${GREEN}Listo!${NC}"
