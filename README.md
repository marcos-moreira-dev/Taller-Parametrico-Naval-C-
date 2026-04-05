# Taller Paramétrico de Navegación y Campos 2D

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)]()
[![Version](https://img.shields.io/badge/version-1.0.0-blue)]()
[![C++](https://img.shields.io/badge/C++-20-orange)]()
[![License](https://img.shields.io/badge/license-Educational-lightgrey)]()

**Aplicación educativa de escritorio para explorar física de fluidos, campos vectoriales y métodos numéricos.**

---

## 📖 DESCRIPCIÓN

Simulador interactivo que combina física computacional, visualización gráfica y contenido educativo para el aprendizaje de:

- **Campos vectoriales** y su dinámica
- **Ecuaciones diferenciales ordinarias (EDOs)**
- **Métodos numéricos** (Euler, Heun, RK4)
- **Física de fluidos** aplicada a navegación

---

## ✨ CARACTERÍSTICAS

### 🌊 Simulación Física
- **Hidrodinámica realista**: Arrastre viscoso/cuadrático, sustentación
- **3 métodos numéricos**: Euler, Euler Mejorado, RK4 con comparación visual
- **10 escenarios predefinidos**: Ríos, remolinos, efecto Venturi, canales
- **Auto-thrust**: Navegación automática optimizada

### 🎨 Visualización
- **Campo vectorial dinámico** con flechas coloridas
- **Sistema de partículas**: Agua, espuma, estela y salpicaduras
- **Gráficas en tiempo real** estilo matplotlib
- **Renderizado de ecuaciones** LaTeX

### 📚 Contenido Educativo
- **14 conceptos teóricos** organizados por categorías
- **Fórmulas interactivas** con explicaciones paso a paso
- **Conexión teoría-práctica**: Botón "Ver en simulación"

### 🛠️ Herramientas
- **Editor de escenarios**: Pintar terreno, obstáculos, campo personalizado
- **Undo/Redo**: Historial de 50 comandos
- **Exportación**: PNG, CSV con trayectorias completas

---

## 🚀 INICIO RÁPIDO

### Windows (MSYS2/MinGW)

```bash
# Clonar repositorio
git clone <repo-url>
cd cplus

# Compilar (automático)
build.bat

# O manualmente:
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build . --target TallerParametrico

# Ejecutar
.\bin\TallerParametrico.exe
```

### Linux/macOS

```bash
# Compilar
./scripts/build.sh

# Ejecutar
./build/bin/TallerParametrico
```

---

## 📁 ESTRUCTURA DEL PROYECTO

```
cplus/
├── src/                          # Código fuente
│   ├── app/                      # Punto de entrada
│   ├── presentation/             # UI (wxWidgets)
│   ├── application/              # Lógica de negocio
│   ├── simulation/               # Física y métodos numéricos
│   ├── domain/                   # Entidades del dominio
│   └── shared/                   # Utilidades comunes
├── tests/                        # Tests unitarios e integración
├── docs/                         # Documentación
├── assets/                       # Recursos (escenarios, teoría)
├── scripts/                      # Scripts de build/utilidades
├── build/                        # Build directory (gitignored)
├── CMakeLists.txt               # Configuración CMake principal
└── README.md                    # Este archivo
```

---

## 🧪 TESTING

```bash
# Ejecutar todos los tests
build.bat test                   # Windows
./scripts/run_all_tests.sh       # Linux/macOS

# O con CTest
cd build
ctest --output-on-failure
```

**Cobertura actual**: 44 tests pasando ✅

---

## 🛠️ DEPENDENCIAS

- **C++20** compatible compiler
- **CMake** 3.16+
- **wxWidgets** 3.3.2+
- **Ninja** o Make

### Instalación de dependencias

**Windows (MSYS2)**:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
```

**Ubuntu/Debian**:
```bash
sudo apt-get install build-essential cmake ninja-build libwxgtk3.2-dev
```

**macOS**:
```bash
brew install cmake ninja wxwidgets
```

---

## 📦 RELEASE

Para crear un instalador MSI:

```bash
# Windows
build.bat release

# MSI (requiere WiX Toolset y variable WIX configurada)
build.bat msi
```

---

## 📚 DOCUMENTACIÓN

- [Arquitectura del sistema](docs/ARCHITECTURE_CONSOLIDATED.md)
- [Guía de wxWidgets](docs/WXWIDGETS_GUIA.md)
- [Especificación del producto](docs/product_spec_proyecto_cpp_taller_parametrico.md)
- [Guía de deploy](docs/DEPLOY.md)
- [Quick Start](docs/QUICKSTART.md)

---

## 🐛 SOLUCIÓN DE PROBLEMAS

### Error: "No se encuentra wxWidgets"

En Windows, `build.bat` intenta reutilizar `build/wx_installed/` o recompilar wxWidgets con `build.bat wx`.
En Linux/macOS, usa la instalación del sistema.

Si falla:

```bash
# Windows: Asegurar que MSYS2 está en PATH
set PATH=C:\msys64\mingw64\bin;%PATH%

# Linux/macOS: Instalar wxWidgets
sudo apt-get install libwxgtk3.2-dev  # Ubuntu
brew install wxwidgets                 # macOS
```

### Error: "DLLs faltantes"

Copiar DLLs de MinGW al directorio del ejecutable:

```bash
cd build/bin
cp /mingw64/bin/libgcc_s_seh-1.dll .
cp /mingw64/bin/libstdc++-6.dll .
cp /mingw64/bin/libwinpthread-1.dll .
```

---

## 🤝 CONTRIBUIR

1. Fork el repositorio
2. Crea una rama (`git checkout -b feature/nueva-funcionalidad`)
3. Commit cambios (`git commit -am 'Añadir nueva funcionalidad'`)
4. Push a la rama (`git push origin feature/nueva-funcionalidad`)
5. Abrir Pull Request

---

## 📝 LICENCIA

Proyecto educativo. Uso permitido para fines educativos y de investigación.

**Bibliotecas de terceros**:
- wxWidgets: LGPL
- C++ Standard Library: Estándar ISO

---

## 👥 AUTORES

Proyecto desarrollado como Taller de C++ Avanzado.

---

## 🙏 AGRADECIMIENTOS

- Comunidad wxWidgets
- Proyecto CMake
- Contribuidores de código abierto

---

<p align="center">⭐ Star este repo si te fue útil!</p>
