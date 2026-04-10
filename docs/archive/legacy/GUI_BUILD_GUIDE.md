# Guía Rápida de Compilación - Taller Paramétrico

## 🚀 Compilación GUI (Versión Recomendada)

### Requisitos Previos

- wxWidgets compilado en `build/wx_installed/`
- CMake 3.20+
- Ninja
- MinGW64 (g++)

### Comandos Rápidos

```powershell
cd "C:\Users\MARCOS MOREIRA\Downloads\cplus\build"
del CMakeCache.txt
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=ON -DBUILD_CONSOLE=OFF -DBUILD_TESTS=OFF
ninja TallerParametrico
```

### Resultado

El ejecutable se generará en:

```
build/bin/TallerParametrico.exe
```

---

## 🔧 Solución de Problemas Comunes

### Error: "unknown target 'TallerParametrico'"

**Causa**: CMake no encuentra wxWidgets en `build/wx_installed/`
**Solución**:

```powershell
# Copiar wxWidgets desde ubicación principal
xcopy /E /I ..\wx_installed .\wx_installed
# O crear enlace simbólico (más eficiente)
mklink /J wx_installed ..\wx_installed
```

### Error: "install TARGETS given target X which does not exist"

**Causa**: Targets condicionales no detectados
**Solución**: Ya corregido en CMakeLists.txt (usa `if(TARGET ...)`)

### Error: "'M_PI' was not declared"

**Causa**: Windows no define M_PI por defecto
**Solución**: Agregado en VectorField.cpp:

```cpp
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
```

### Error: "'map' in namespace 'std' does not name a template type"

**Causa**: Falta `#include <map>`
**Solución**: Agregado en EquationRenderer.cpp

---

## 📋 Checklist Pre-Compilación

- [ ] wxWidgets está en `build/wx_installed/`
- [ ] CMakeCache.txt borrado (para configuración limpia)
- [ ] No hay archivos objeto corruptos

---

## 🎯 Scripts Disponibles

- `build_gui_final.bat` - Script completo con manejo de errores
- `build_gui_rapido.bat` - Recompilación rápida (sin reconfigurar)

**Autor**: Asistente Claude Code  
**Fecha**: Marzo 2026  
**Versión**: 1.0
