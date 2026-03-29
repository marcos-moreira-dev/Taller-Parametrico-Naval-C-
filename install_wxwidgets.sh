#!/bin/bash
# Instalador de wxWidgets para MSYS2 MinGW64

echo "========================================"
echo "  INSTALADOR DE WXWIDGETS"
echo "========================================"
echo ""

# Verificar que estamos en MinGW
if [[ "$MSYSTEM" != "MINGW64" ]]; then
    echo "[ERROR] Debes estar en MinGW64 shell"
    echo "[INFO] Abre MSYS2 MinGW64 y ejecuta este script"
    exit 1
fi

echo "[INFO] Instalando wxWidgets con pacman..."
echo ""

# Actualizar primero
pacman -Sy

# Instalar wxWidgets
pacman -S --needed --noconfirm mingw-w64-x86_64-wxWidgets3.2

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================"
    echo "[SUCCESS] wxWidgets instalado!"
    echo "========================================"
    echo ""
    echo "Ahora puedes compilar el proyecto:"
    echo "  ./build_gui.sh"
    echo ""
else
    echo ""
    echo "[ERROR] Fallo la instalacion"
    exit 1
fi
