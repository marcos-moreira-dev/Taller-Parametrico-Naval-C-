# Guía de Compilación de wxWidgets para Taller Paramétrico

## Resumen

Esta guía documenta los pasos exactos utilizados para compilar wxWidgets 3.3.2 y configurar el proyecto Taller Paramétrico con soporte GUI.

## Requisitos

- **Sistema**: Windows 10/11
- **Compilador**: MinGW-w64 (GCC 14.2.0)
- **CMake**: 3.20 o superior
- **wxWidgets**: Versión 3.3.2 (descargada desde GitHub)

## Estructura de Directorios

```
cplus/
├── build/
│   ├── wxwidgets/          # Build temporal de wxWidgets
│   ├── wx_installed/       # Instalación final de wxWidgets
│   └── bin/
│       └── TallerParametrico.exe
└── src/
    └── presentation/       # Código GUI
```

## Pasos de Compilación

### 1. Descargar wxWidgets

```bash
# Descargar desde GitHub
wget https://github.com/wxWidgets/wxWidgets/archive/refs/tags/v3.3.2.tar.gz

# O clonar el repositorio
git clone https://github.com/wxWidgets/wxWidgets.git
cd wxWidgets
git checkout v3.3.2
```

### 2. Configurar CMake

```bash
# Crear directorio de build
mkdir build/wxwidgets
cd build/wxwidgets

# Configurar con MinGW (flags importantes)
cmake ../../wxWidgets-3.3.2 \
    -G "Ninja" \
    -DCMAKE_BUILD_TYPE=Release \
    -DwxBUILD_SHARED=OFF \
    -DCMAKE_INSTALL_PREFIX="../wx_installed" \
    -DwxUSE_WEBVIEW=OFF \
    -DwxUSE_MEDIACTRL=OFF
```

**Flags importantes:**

- `wxBUILD_SHARED=OFF`: Compilar librerías estáticas
- `wxUSE_WEBVIEW=OFF`: Desactivar WebView (reduce dependencias)
- `wxUSE_MEDIACTRL=OFF`: Desactivar controles multimedia

### 3. Compilar wxWidgets

```bash
# Compilar (toma 10-15 minutos)
cmake --build . --parallel 2
```

### 4. Instalar wxWidgets

```bash
# Instalar en directorio final
cmake --install . --prefix "../wx_installed"
```

### 5. Configurar Proyecto Principal

Archivo `CMakeLists.txt`:

```cmake
if(BUILD_GUI)
    set(WX_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build/wx_installed")

    if(EXISTS "${WX_INSTALL_DIR}/include/wx/wx.h")
        set(wxWidgets_FOUND TRUE)
        set(wxWidgets_ROOT_DIR ${WX_INSTALL_DIR})
        set(wxWidgets_INCLUDE_DIRS ${WX_INSTALL_DIR}/include)
        set(wxWidgets_LIBRARY_DIRS ${WX_INSTALL_DIR}/lib/gcc_x64_lib)

        # Librerías requeridas
        set(wxWidgets_LIBRARIES
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxmsw33u_core.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxbase33u.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxbase33u_net.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxpng.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxjpeg.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxzlib.a
            ${WX_INSTALL_DIR}/lib/gcc_x64_lib/wxregexu.a
            # Librerías de Windows necesarias
            comctl32 uuid uxtheme version shlwapi gdi32 gdiplus msimg32 oleacc
        )

        add_subdirectory(src/presentation)
    endif()
endif()
```

### 6. Incluir Directorios

En `src/presentation/CMakeLists.txt`:

```cmake
target_include_directories(tp_presentation
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${wxWidgets_INCLUDE_DIRS}
        ${wxWidgets_LIBRARY_DIRS}/mswu  # Para wx/setup.h
)

target_link_libraries(tp_presentation
    PUBLIC
        tp_application
        tp_shared
        ${wxWidgets_LIBRARIES}
)
```

### 7. Compilar Proyecto

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=ON
cmake --build . --parallel 2
```

## Solución de Problemas

### Error: `wx/setup.h` no encontrado

**Solución**: Agregar al include path:

```cmake
${wxWidgets_LIBRARY_DIRS}/mswu
```

### Error: `ssize_t` no definido

**Solución**: Definir antes de incluir wxWidgets:

```cpp
#define _ssize_t long
#include <wx/wx.h>
```

### Error: Símbolos no definidos (Gdiplus, UxTheme, etc.)

**Solución**: Agregar librerías de Windows:

```cmake
comctl32 uuid uxtheme version shlwapi gdi32 gdiplus msimg32 oleacc
```

### Error: `wx/core` no encontrado

**Solución**: Usar nombres exactos de las librerías:

- `wxmsw33u_core.a` (no wx_core)
- `wxbase33u.a` (no wx_base)

## Librerías Generadas

En `build/wx_installed/lib/gcc_x64_lib/`:

- `wxmsw33u_core.a` - Componentes core GUI
- `wxbase33u.a` - Funcionalidad base
- `wxbase33u_net.a` - Soporte de red
- `wxmsw33u_adv.a` - Controles avanzados
- `wxmsw33u_aui.a` - Interfaz dockable
- `wxpng.a`, `wxjpeg.a`, `wxzlib.a` - Librerías de imagen
- `wxregexu.a` - Expresiones regulares

## Configuración de la Ventana

### Estilo de Ventana Nativo

```cpp
MainWindow::MainWindow(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1200, 800))
    // Usa wxDEFAULT_FRAME_STYLE implícitamente
```

**Estilos disponibles:**

- `wxCAPTION`: Barra de título
- `wxMINIMIZE_BOX`: Botón minimizar
- `wxMAXIMIZE_BOX`: Botón maximizar
- `wxCLOSE_BOX`: Botón cerrar
- `wxRESIZE_BORDER`: Bordes redimensionables
- `wxSYSTEM_MENU`: Menú de sistema

### Solución de Problemas de Ventana

Si la ventana no tiene controles nativos:

1. **No crear paneles de título manual**: Usar la barra de título nativa
2. **No sobreescribir `wxDefaultPosition`**: Permite que Windows gestione la posición
3. **Usar `Centre()`**: Centra la ventana correctamente

## Dependencias de Windows

Librerías del sistema necesarias:

```
kernel32
user32
gdi32
winspool
shell32
ole32
oleaut32
uuid
comdlg32
advapi32
comctl32      # Controles comunes
uxtheme         # Temas visuales
version         # Información de versión
shlwapi         # Shell API
gdiplus         # GDI+
msimg32         # Image manipulation
oleacc          # Accessibility
```

## Script de Compilación Automatizado

Archivo `build_wxwidgets.bat`:

```batch
@echo off
set WX_VERSION=3.3.2
set WX_SOURCE=wxWidgets-%WX_VERSION%
set BUILD_DIR=build\wxwidgets
set INSTALL_DIR=build\wx_installed

echo [1/4] Configurando wxWidgets...
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%
cmake ..\..\%WX_SOURCE% ^
    -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DwxBUILD_SHARED=OFF ^
    -DCMAKE_INSTALL_PREFIX=..\..\%INSTALL_DIR% ^
    -DwxUSE_WEBVIEW=OFF ^
    -DwxUSE_MEDIACTRL=OFF

echo [2/4] Compilando wxWidgets...
cmake --build . --parallel 2

echo [3/4] Instalando wxWidgets...
cmake --install . --prefix ..\..\%INSTALL_DIR%

echo [4/4] Listo!
echo Instalado en: %INSTALL_DIR%
```

## Referencias

- [wxWidgets Documentation](https://docs.wxwidgets.org/)
- [wxWidgets GitHub](https://github.com/wxWidgets/wxWidgets)
- [MinGW-w64](https://www.mingw-w64.org/)

## Notas

- Tiempo de compilación: ~10-15 minutos en CPU moderno
- Tamaño final del ejecutable: ~13-14 MB (con wxWidgets estático)
- La compilación estática evita dependencias de DLLs
