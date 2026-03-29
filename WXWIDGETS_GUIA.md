# SOLUCIÓN RÁPIDA PARA WXWIDGETS

## Opción 1: Usar Versión Precompilada (Recomendado)

Descarga wxWidgets ya compilado desde:
https://github.com/wxWidgets/wxWidgets/releases

1. Ve a: https://github.com/wxWidgets/wxWidgets/releases
2. Descarga: `wxWidgets-3.2.4-headers.7z`
3. Descarga: `wxWidgets-3.2.4-vc14x_x64_dll.7z` (librerías DLL)
4. Extrae ambos en: `C:\msys64\mingw64\`

## Opción 2: Compilar Automáticamente (Lento)

Ejecuta en PowerShell:

```powershell
.\build_with_wx.bat
```

Este script:

1. Compila wxWidgets (15-20 minutos)
2. Compila el proyecto con GUI
3. Genera `TallerParametrico.exe`

## Opción 3: Usar Versión Consola (Inmediata)

Ya tienes funcionando:

```powershell
.\run.bat
```

Esta versión tiene menú interactivo y todas las funcionalidades excepto la interfaz gráfica wxWidgets.

## Estructura del Proyecto GUI

Cuando wxWidgets esté disponible, el proyecto incluirá:

- **Ventana principal** con menú, toolbar y barra de estado
- **Canvas central** para visualizar el escenario
- **Panel izquierdo**: Navegación y lista de escenarios
- **Panel derecho**: Inspector de propiedades
- **Panel inferior**: Pestañas de resultados y gráficas
- **Modo Teoría**: Fichas didácticas tipo Encarta
- **Modo Simulación**: Controles de reproducción
- **Modo Editor**: Herramientas de diseño de escenarios

## Requisitos para GUI

- wxWidgets 3.2+ compilado
- Headers en: `C:\msys64\mingw64\include\wx`
- Librerías en: `C:\msys64\mingw64\lib`
- CMake configurado con: `-DBUILD_GUI=ON`

## Problemas Conocidos

Si CMake no encuentra wxWidgets, verifica:

1. Variable de entorno `WXWIN` apunta a la carpeta de wxWidgets
2. Librerías están en `lib/gcc_x64_64_dll/` o `lib/vc_x64_dll/`
3. Headers están en `include/wx/`

## Contacto

Si tienes problemas, revisa los logs en:

- `build/CMakeFiles/CMakeError.log`
- `build/CMakeFiles/CMakeOutput.log`
