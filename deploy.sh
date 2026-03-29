#!/bin/bash

# Script de Despliegue - Taller Parametrico de Navegacion y Campos 2D
# Este script automatiza la compilacion, pruebas y despliegue del proyecto

set -e  # Exit on error

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Variables
PROJECT_NAME="TallerParametrico"
BUILD_TYPE="Debug"
PRESET="dev-debug"
LOG_FILE="logs/deploy_$(date +%Y%m%d_%H%M%S).log"

# Funciones de logging
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1" | tee -a "$LOG_FILE"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1" | tee -a "$LOG_FILE"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1" | tee -a "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$LOG_FILE"
}

# Funcion para verificar dependencias
check_dependencies() {
    log_info "Verificando dependencias..."
    
    if ! command -v cmake &> /dev/null; then
        log_error "CMake no esta instalado. Por favor instale CMake 3.20+"
        exit 1
    fi
    
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    log_info "CMake version: $CMAKE_VERSION"
    
    if ! command -v vcpkg &> /dev/null; then
        log_warning "vcpkg no encontrado en PATH. Asegurese de tener vcpkg instalado."
        log_warning "Si usa vcpkg, configure la variable de entorno VCPKG_ROOT"
    fi
    
    # Verificar ninja
    if command -v ninja &> /dev/null; then
        log_info "Ninja encontrado"
        export CMAKE_GENERATOR="Ninja"
    else
        log_warning "Ninja no encontrado. Usando generador por defecto."
    fi
    
    log_success "Dependencias verificadas"
}

# Funcion para limpiar builds anteriores
clean_build() {
    log_info "Limpiando builds anteriores..."
    
    if [ -d "build" ]; then
        rm -rf build
        log_success "Directorio build eliminado"
    fi
    
    # Crear directorio de logs
    mkdir -p logs
    
    log_success "Limpieza completada"
}

# Funcion para configurar el proyecto
configure_project() {
    log_info "Configurando proyecto con preset: $PRESET..."
    
    # Intentar con preset primero
    if cmake --preset "$PRESET" 2>&1 | tee -a "$LOG_FILE"; then
        log_success "Configuracion con preset completada exitosamente"
    else
        log_warning "Fallo la configuracion con preset. Intentando configuracion manual..."
        
        mkdir -p "build/$PRESET"
        cd "build/$PRESET"
        
        if cmake ../.. \
            -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
            -DBUILD_TESTS=ON \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 2>&1 | tee -a "../../$LOG_FILE"; then
            log_success "Configuracion manual completada exitosamente"
        else
            log_error "Error en la configuracion del proyecto"
            exit 1
        fi
        
        cd ../..
    fi
}

# Funcion para compilar el proyecto
build_project() {
    log_info "Compilando proyecto..."
    
    if cmake --build "build/$PRESET" --parallel 2>&1 | tee -a "$LOG_FILE"; then
        log_success "Compilacion completada exitosamente"
    else
        log_error "Error en la compilacion"
        exit 1
    fi
}

# Funcion para ejecutar tests
run_tests() {
    log_info "Ejecutando tests..."
    
    cd "build/$PRESET"
    if ctest --output-on-failure 2>&1 | tee -a "../../$LOG_FILE"; then
        log_success "Todos los tests pasaron exitosamente"
    else
        log_warning "Algunos tests fallaron. Revise los logs."
    fi
    cd ../..
}

# Funcion para ejecutar la aplicacion
run_application() {
    log_info "Iniciando aplicacion..."
    
    EXECUTABLE="build/$PRESET/bin/$PROJECT_NAME"
    
    if [ -f "$EXECUTABLE" ]; then
        log_success "Ejecutable encontrado: $EXECUTABLE"
        log_info "Iniciando Taller Parametrico..."
        echo -e "${GREEN}========================================${NC}"
        echo -e "${GREEN}  TALLER PARAMETRICO - NAVEGACION 2D  ${NC}"
        echo -e "${GREEN}========================================${NC}"
        echo ""
        
        # Ejecutar la aplicacion
        ./"$EXECUTABLE" 2>&1 | tee -a "$LOG_FILE"
    else
        log_error "Ejecutable no encontrado: $EXECUTABLE"
        # Buscar ejecutable alternativo
        log_info "Buscando ejecutable alternativo..."
        find "build/$PRESET" -name "$PROJECT_NAME" -type f 2>/dev/null | head -1
        exit 1
    fi
}

# Funcion para mostrar estadisticas
show_stats() {
    echo ""
    log_info "Estadisticas del Build:"
    echo "  - Archivos de log: $LOG_FILE"
    echo "  - Directorio de build: build/$PRESET"
    echo "  - Ejecutable: build/$PRESET/bin/$PROJECT_NAME"
    
    if [ -d "build/$PRESET" ]; then
        BUILD_SIZE=$(du -sh build/$PRESET 2>/dev/null | cut -f1 || echo "Desconocido")
        echo "  - Tamanio del build: $BUILD_SIZE"
    fi
}

# Funcion para mostrar ayuda
show_help() {
    echo "Uso: $0 [opciones]"
    echo ""
    echo "Opciones:"
    echo "  -h, --help       Muestra esta ayuda"
    echo "  -c, --clean      Limpia el build antes de compilar"
    echo "  -t, --test       Ejecuta tests despues de compilar"
    echo "  -r, --run        Ejecuta la aplicacion despues de compilar"
    echo "  -a, --all        Limpia, compila, testea y ejecuta (todo)"
    echo "  --release        Usa configuracion Release (por defecto: Debug)"
    echo ""
    echo "Ejemplos:"
    echo "  $0               Solo compila"
    echo "  $0 -c            Limpia y compila"
    echo "  $0 -t            Compila y ejecuta tests"
    echo "  $0 -r            Compila y ejecuta la aplicacion"
    echo "  $0 -a            Hace todo: limpia, compila, testea y ejecuta"
}

# Menu principal
main() {
    local CLEAN=0
    local TEST=0
    local RUN=0
    local ALL=0
    
    # Parsear argumentos
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--clean)
                CLEAN=1
                shift
                ;;
            -t|--test)
                TEST=1
                shift
                ;;
            -r|--run)
                RUN=1
                shift
                ;;
            -a|--all)
                ALL=1
                CLEAN=1
                TEST=1
                RUN=1
                shift
                ;;
            --release)
                BUILD_TYPE="Release"
                PRESET="dev-release"
                shift
                ;;
            *)
                log_error "Opcion desconocida: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Mostrar banner
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}  SCRIPT DE DESPLIEGUE - TALLER       ${NC}"
    echo -e "${GREEN}  PARAMETRICO DE NAVEGACION 2D       ${NC}"
    echo -e "${GREEN}========================================${NC}"
    echo ""
    
    log_info "Iniciando proceso de despliegue..."
    log_info "Modo: $BUILD_TYPE"
    log_info "Preset: $PRESET"
    log_info "Log: $LOG_FILE"
    echo ""
    
    # Verificar dependencias
    check_dependencies
    echo ""
    
    # Limpiar si es necesario
    if [ $CLEAN -eq 1 ]; then
        clean_build
        echo ""
    fi
    
    # Configurar
    configure_project
    echo ""
    
    # Compilar
    build_project
    echo ""
    
    # Ejecutar tests si es necesario
    if [ $TEST -eq 1 ]; then
        run_tests
        echo ""
    fi
    
    # Mostrar estadisticas
    show_stats
    echo ""
    
    # Ejecutar aplicacion si es necesario
    if [ $RUN -eq 1 ]; then
        run_application
    else
        log_success "Proceso de despliegue completado!"
        log_info "Para ejecutar la aplicacion: ./$0 -r"
    fi
}

# Ejecutar main
main "$@"
