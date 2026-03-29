# Taller Paramétrico de Navegación y Campos 2D

**Versión**: 0.2.0  
**Autor**: Proyecto de C++  
**Licencia**: Proyecto Educativo

---

## 📖 DESCRIPCIÓN

Aplicación de escritorio en C++ para la exploración didáctica de campos vectoriales, trayectorias y métodos numéricos en escenarios 2D editables.

Combina:

- **Simulación física** con hidrodinámica realista
- **Visualización interactiva** con efectos de agua
- **Contenido educativo** integrado
- **Experimentación** paramétrica

---

## 🎯 CARACTERÍSTICAS PRINCIPALES

### 🌊 Simulación Física

- **Hidrodinámica avanzada**: Arrastre viscoso/cuadrático, sustentación, Coriolis
- **3 métodos numéricos**: Euler, Euler Mejorado, RK4
- **Física en tiempo real**: Cambio de parámetros durante simulación
- **6 escenarios predefinidos**: Ríos, remolinos, canales, laberintos

### 🎨 Visualización

- **Campo vectorial dinámico**: Flechas coloridas con dirección/intensidad
- **Sistema de partículas**: Agua, espuma y estela del bote
- **Gráficas en tiempo real**: Velocidad, energía, posición
- **Trayectoria animada**: Rastro del movimiento con colores

### 📚 Contenido Educativo

- **14 conceptos teóricos**: Campos, EDOs, métodos numéricos, física
- **Navegación por categorías**: Cálculo, Física, Métodos Numéricos
- **Fórmulas interactivas**: Renderizado con ejemplos
- **Conexión simulación-teoría**: Botón "Ver en simulación"

### 🛠️ Herramientas

- **Editor de escenarios**: Pintar agua/tierra/obstáculos
- **Campo personalizado**: Definir funciones matemáticas
- **Exportación**: Imágenes PNG/JPG y datos CSV
- **Gestión de archivos**: Guardar/cargar escenarios

---

## 🚀 INSTALACIÓN

### Requisitos

- Windows 7/10/11 (64-bit)
- Visual C++ Redistributable 2015-2022
- MinGW-w64 (opcional, para compilación)

### Instalación Rápida

1. Descargar `TallerParametrico.zip`
2. Extraer en carpeta deseada
3. Ejecutar `TallerParametrico.exe`

**Nota**: Si aparece error de DLLs faltantes, instalar:
[Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe)

---

## 🎮 USO RÁPIDO

### Controles Básicos

```
Espacio        - Iniciar/Pausar simulación
R              - Reiniciar simulación
F1/F2/F3       - Cambiar modo (Teoría/Simulación/Editor)
Ctrl+N/O/S     - Nuevo/Abrir/Guardar
Ctrl+E         - Exportar imagen
```

### Primeros Pasos

1. **Iniciar**: Presionar `Espacio` para comenzar simulación
2. **Ajustar parámetros**: Mover sliders en panel derecho
3. **Cambiar escenario**: Seleccionar de lista en panel izquierdo
4. **Ver teoría**: Presionar F1 y explorar conceptos

### Ejemplo: Comparar Métodos Numéricos

1. Seleccionar escenario "Remolino central"
2. Iniciar simulación con Euler
3. Cambiar a RK4 sin pausar
4. Observar diferencias en trayectoria

---

## 📐 FUNDAMENTOS MATEMÁTICOS

### Modelo del Bote

**Ecuaciones de movimiento**:

```
m·dv/dt = F_motor + F_arrastre + F_campo
I·dω/dt = τ_alineación + τ_amortiguamiento
```

**Fuerzas hidrodinámicas**:

- **Arrastre viscoso**: F = -b·v (Re < 1000)
- **Arrastre cuadrático**: F = -½·ρ·Cd·A·v² (Re > 100000)
- **Sustentación**: Función del número de Froude
- **Coriolis**: F = 2m·ω×v

### Métodos Numéricos

| Método         | Error | Uso Recomendado             |
| -------------- | ----- | --------------------------- |
| Euler          | O(h²) | Exploración rápida          |
| Euler Mejorado | O(h³) | Balance precisión/velocidad |
| RK4            | O(h⁵) | Precisión máxima            |

---

## 🏗️ ARQUITECTURA

### Capas del Sistema

```
┌─────────────────────────────────────────┐
│  Presentation (wxWidgets GUI)           │
│  - MainWindow, Paneles, Canvas          │
├─────────────────────────────────────────┤
│  Application (ExperimentService)        │
│  - Estado, Configuración, Persistencia  │
├─────────────────────────────────────────┤
│  Simulation (Integradores)              │
│  - Euler, RK4, Física                   │
├─────────────────────────────────────────┤
│  Domain (Entidades)                     │
│  - Boat, VectorField, Scenario          │
├─────────────────────────────────────────┤
│  Shared (Utilidades)                    │
│  - Vec2, Logger, Enums                  │
└─────────────────────────────────────────┘
```

### Tecnologías

- **C++20**: Lenguaje principal
- **wxWidgets 3.3.2**: Framework GUI
- **CMake**: Sistema de build
- **STL**: Contenedores y algoritmos

---

## 📁 ESTRUCTURA DE ARCHIVOS

```
cplus/
├── src/
│   ├── app/                    # Punto de entrada
│   │   ├── main.cpp           # GUI
│   │   └── console_main.cpp   # Versión consola
│   ├── presentation/          # UI
│   │   ├── main_window/       # Ventana principal
│   │   ├── panels/           # Paneles (Left, Right, Bottom)
│   │   └── widgets/          # Canvas, Gráficas, Partículas
│   ├── application/           # Lógica de app
│   │   └── ExperimentService
│   ├── simulation/            # Física
│   │   ├── integrators/      # Euler, RK4
│   │   └── runner/           # Control simulación
│   ├── domain/               # Entidades
│   │   ├── Boat.hpp         # Modelo hidrodinámico
│   │   ├── VectorField.hpp  # Campo vectorial
│   │   └── Scenario.hpp     # Mapa 2D
│   └── shared/               # Utilidades
│       ├── Vec2.hpp         # Vector 2D
│       └── Logger.hpp       # Logging
├── docs/                     # Documentación
│   ├── ARCHITECTURE_CONSOLIDATED.md
│   └── product_spec_proyecto_cpp_taller_parametrico.md
├── build/                    # Archivos de compilación
│   ├── bin/                 # Ejecutables
│   │   ├── TallerParametrico.exe
│   │   ├── libgcc_s_seh-1.dll
│   │   ├── libstdc++-6.dll
│   │   └── libwinpthread-1.dll
│   └── wx_installed/        # Librerías wxWidgets
└── CMakeLists.txt           # Configuración CMake
```

---

## 🔧 COMPILACIÓN

### Desde MSYS2/MinGW

```bash
cd cplus/build
cmake .. -G Ninja
cmake --build . --target TallerParametrico
```

### Desde Visual Studio

```bash
cd cplus/build
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" .. -G Ninja
cmake --build . --target TallerParametrico
```

---

## 📊 RENDIMIENTO

- **FPS Objetivo**: 30 (configurable)
- **Partículas máximas**: 800
- **Resolución escenario**: Hasta 200x200 celdas
- **Memoria**: ~50MB en ejecución

---

## 🐛 SOLUCIÓN DE PROBLEMAS

### Error: "No se encuentran DLLs"

**Solución**: Instalar Visual C++ Redistributable o copiar DLLs de MinGW a carpeta bin/

### Error: "Ventana sin botones cerrar/minimizar"

**Solución**: Es un bug conocido de wxWidgets+AUI. Usar Alt+F4 para cerrar.

### Simulación muy lenta

**Solución**: Reducir paso temporal o cambiar a método Euler

---

## 📝 LICENCIA Y ATRIBUTOS

Proyecto educativo desarrollado como taller de C++.

### Bibliotecas de Terceros

- wxWidgets (LGPL)
- STL (Standard C++ Library)

---

## 🤝 CONTRIBUCIONES

Áreas para extensión:

1. Nuevos tipos de campos vectoriales
2. Más escenarios predefinidos
3. Soporte para 3D
4. Multiplayer colaborativo
5. Integración con OpenGL

---

## 📞 SOPORTE

Para reportar bugs o sugerencias:

- GitHub Issues: https://github.com/anomalyco/opencode/issues

---

**Última actualización**: Marzo 2025  
**Versión documentación**: 1.0
