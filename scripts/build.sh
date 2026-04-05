#!/bin/bash
# ============================================================================
# build.sh - Script de build unificado para Linux/macOS
# ============================================================================
# Uso:
#   ./build.sh              # Build GUI (por defecto)
#   ./build.sh run          # Build y ejecutar
#   ./build.sh rebuild      # Limpia y recompila
#   ./build.sh clean        # Limpia build
#   ./build.sh console      # Build versión consola
#   ./build.sh tests        # Build con tests
#   ./build.sh release      # Build de release
# ============================================================================

set -e  # Exit on error

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directorios
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$SCRIPT_DIR"
BUILD_DIR="$ROOT_DIR/build"

# Defaults
TARGET="TallerParametrico"
BUILD_GUI="ON"
BUILD_CONSOLE="OFF"
BUILD_TESTS="OFF"
BUILD_TYPE="Release"
ACTION="build"
RUN_AFTER=0

# Parsear argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
        run)
            RUN_AFTER=1
            shift
            ;;
        rebuild)
            ACTION="rebuild"
            shift
            ;;
        clean)
            ACTION="clean"
            shift
            ;;
        console)
            TARGET="TallerParametrico_Console"
            BUILD_GUI="OFF"
            BUILD_CONSOLE="ON"
            shift
            ;;
        tests)
            BUILD_TESTS="ON"
            shift
            ;;
        release)
            BUILD_TYPE="Release"
            shift
            ;;
        debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        help|-h|--help)
            echo "Uso: $0 [opciones]"
            echo ""
            echo "Opciones:"
            echo "  run       - Compila y ejecuta"
            echo "  rebuild   - Limpia y recompila"
            echo "  clean     - Limpia el directorio de build"
            echo "  console   - Compila versión consola"
            echo "  tests     - Compila con tests habilitados"
            echo "  release   - Build de release (por defecto)"
            echo "  debug     - Build de debug"
            echo "  help      - Muestra esta ayuda"
            exit 0
            ;;
        *)
            echo -e "${RED}Opción desconocida: $1${NC}"
            exit 1
            ;;
    esac
done

echo "========================================"
echo "  Taller Paramétrico - Build"
echo "========================================"
echo ""

# Acción: clean
if [ "$ACTION" == "clean" ]; then
    echo -e "${YELLOW}[INFO] Limpiando build...${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}[OK] Build limpiado${NC}"
    exit 0
fi

# Acción: rebuild
if [ "$ACTION" == "rebuild" ]; then
    echo -e "${YELLOW}[INFO] Rebuild solicitado. Limpiando...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Crear directorio de build
mkdir -p "$BUILD_DIR"

# Configurar con CMake si es necesario
if [ ! -f "$BUILD_DIR/build.ninja" ] || [ "$ACTION" == "rebuild" ]; then
    echo -e "${YELLOW}[INFO] Configurando CMake...${NC}"
    cmake -S "$ROOT_DIR" \
          -B "$BUILD_DIR" \
          -G Ninja \
          -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
          -DBUILD_GUI="$BUILD_GUI" \
          -DBUILD_CONSOLE="$BUILD_CONSOLE" \
          -DBUILD_TESTS="$BUILD_TESTS"
fi

# Compilar
echo -e "${YELLOW}[INFO] Compilando $TARGET...${NC}"
ninja -C "$BUILD_DIR" "$TARGET"

# Verificar que se creó el ejecutable
if [ ! -f "$BUILD_DIR/bin/$TARGET" ]; then
    echo -e "${RED}[ERROR] No se encontró el ejecutable${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}[OK] Build exitoso${NC}"
echo -e "${GREEN}[OK] Ejecutable: $BUILD_DIR/bin/$TARGET${NC}"

# Ejecutar si se solicitó
if [ $RUN_AFTER -eq 1 ]; then
    echo ""
    echo -e "${YELLOW}[INFO] Ejecutando $TARGET...${NC}"
    "$BUILD_DIR/bin/$TARGET"
fi
