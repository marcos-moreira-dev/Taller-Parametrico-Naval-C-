# ARQUITECTURA TÉCNICA - Taller Paramétrico de Navegación y Campos 2D

## 1. VISTA GENERAL DEL SISTEMA

```
┌─────────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  MainWindow  │  │ TheoryPanel  │  │ EditorPanel  │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  LeftPanel   │  │ RightPanel   │  │ BottomPanel  │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│  ┌─────────────────────────────────────────────────────┐    │
│  │              SimulationCanvas                        │    │
│  │  ┌──────────────┐  ┌──────────────┐                │    │
│  │  │ SimpleGraph  │  │ WaterParticle│                │    │
│  │  └──────────────┘  └──────────────┘                │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                   APPLICATION LAYER                          │
│                    ExperimentService                         │
│         (gestión de estado y configuración)                  │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    SIMULATION LAYER                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  Integrators │  │PhysicsModel  │  │  Runner      │       │
│  │  (Euler/RK4) │  │ (fuerzas)    │  │ (control)    │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      DOMAIN LAYER                            │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ Scenario │  │VectorField│  │   Boat   │  │  Motor   │    │
│  │ (mapa)   │  │ (fluido)  │  │ (objeto) │  │(propuls) │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      SHARED LAYER                            │
│              Vec2, Logger, Enums, Utils                      │
└─────────────────────────────────────────────────────────────┘
```

## 2. CAPAS DEL SISTEMA

### 2.1 Presentation Layer (`src/presentation/`)

**Responsabilidad**: Interfaz gráfica e interacción con usuario

**Componentes Principales**:

#### MainWindow

- **Archivo**: `main_window/MainWindow.cpp/hpp`
- **Propósito**: Ventana principal, gestión de layout AUI
- **Dependencias**: Todos los paneles, ExperimentService
- **Patrón**: Singleton (una instancia), Observer (eventos)

#### Paneles

| Panel       | Archivo              | Propósito                                     |
| ----------- | -------------------- | --------------------------------------------- |
| LeftPanel   | `panels/LeftPanel`   | Navegación, lista de escenarios, capas        |
| RightPanel  | `panels/RightPanel`  | Inspector de propiedades (Motor, Campo, Bote) |
| BottomPanel | `panels/BottomPanel` | Resultados, gráficas, eventos, teoría         |
| TheoryPanel | `panels/TheoryPanel` | Contenido educativo con 14 conceptos          |
| EditorPanel | `panels/EditorPanel` | Herramientas de edición de escenarios         |

#### Widgets

| Widget              | Archivo                       | Propósito                     |
| ------------------- | ----------------------------- | ----------------------------- |
| SimulationCanvas    | `widgets/SimulationCanvas`    | Renderizado 2D del escenario  |
| SimpleGraph         | `widgets/SimpleGraph`         | Gráficas de velocidad/energía |
| WaterParticleSystem | `widgets/WaterParticleSystem` | Efectos de agua y espuma      |
| ToastNotification   | `widgets/ToastNotification`   | Notificaciones emergentes     |

### 2.2 Application Layer (`src/application/`)

**Responsabilidad**: Lógica de aplicación y estado

#### ExperimentService

```cpp
class ExperimentService {
    ExperimentConfig config_;  // Configuración actual

    void createNewExperiment();
    void loadExperiment(path);
    void saveExperiment(path);
    ExperimentConfig& getConfig();
};
```

**Estructura de Configuración**:

```cpp
struct ExperimentConfig {
    Scenario scenario;              // Mapa 2D
    VectorField field;              // Campo de velocidades
    Boat boat;                      // Objeto navegable
    Motor motor;                    // Sistema de propulsión
    IntegrationMethod method;       // Método numérico
    double timeStep;                // Δt
    double maxTime;                 // Tiempo máximo
};
```

### 2.3 Simulation Layer (`src/simulation/`)

**Responsabilidad**: Cálculos físicos y numéricos

#### Integradores

```cpp
// Interfaz común
class IIntegrator {
    virtual State integrate(const State& current, double dt,
                           const PhysicsModel& model) = 0;
};

// Implementaciones
class EulerIntegrator : public IIntegrator;          // O(h²) error
class ImprovedEulerIntegrator : public IIntegrator;  // O(h³) error
class RK4Integrator : public IIntegrator;            // O(h⁵) error
```

#### PhysicsModel

- Calcula fuerzas: motor + arrastre + campo
- Aplica ecuaciones de movimiento
- Detecta colisiones

#### SimulationRunner

- Orquesta la simulación paso a paso
- Maneja el tiempo y estados
- Colecciona resultados

### 2.4 Domain Layer (`src/domain/`)

**Responsabilidad**: Entidades del negocio (POCOs)

#### Boat - Modelo Hidrodinámico

```cpp
class Boat {
    // Estado
    Vec2d position, velocity;
    double orientation, angularVelocity;

    // Propiedades físicas
    double mass, size, dragCoefficient;
    HullProperties hull;  // Longitud, manga, calado
    double inertia;       // Momento de inercia

    // Comportamiento
    void applyHydrodynamicForces(Vec2d waterVel, double density, double dt);
    void applyForce(Vec2d force, double dt);
    void applyTorque(double torque, double dt);
};
```

**Física Implementada**:

- Arrastre viscoso (bajas velocidades)
- Arrastre cuadrático (altas velocidades)
- Número de Reynolds (transición)
- Sustentación hidrodinámica (planeo)
- Fuerza de Coriolis
- Torque de alineación

#### VectorField - Campo Vectorial

```cpp
class VectorField {
    Vec2d getValue(double x, double y) const;

    // Fábricas
    static VectorField uniform(double intensity, double angle);
    static VectorField radial(double cx, double cy, double intensity);
    static VectorField rotational(double cx, double cy, double intensity);
    static VectorField linear(double ax, double ay, double bx, double by);
};
```

#### Scenario - Mapa 2D

```cpp
class Scenario {
    Grid<CellType> cells;  // Water, Land, Obstacle
    int width, height;

    void setCell(int x, int y, CellType type);
    CellType getCell(int x, int y) const;
};
```

### 2.5 Shared Layer (`src/shared/`)

**Responsabilidad**: Utilidades comunes

```cpp
// Vec2.hpp - Vector 2D template
// Logger.hpp - Sistema de logging
// Enums.hpp - Enumeraciones compartidas
```

## 3. FLUJO DE DATOS

### 3.1 Inicialización

```
main()
  → MainWindow
    → ExperimentService::createNewExperiment()
      → Configuración por defecto
    → Setup UI (paneles AUI)
    → Cargar escenario inicial
```

### 3.2 Simulación (Loop Principal)

```
Timer (30 FPS)
  → MainWindow::runSimulationStep()
    → Leer parámetros UI (en tiempo real)
    → SimulationRunner
      → PhysicsModel::calculateForces()
        → Boat::applyHydrodynamicForces()
      → Integrator::integrate()
    → Actualizar estado bote
    → Actualizar gráficas
    → Renderizar canvas
```

### 3.3 Eventos de Usuario

```
Usuario mueve slider
  → Evento wxWidgets
    → RightPanel::updateMotorFromUI()
      → ExperimentService::getConfig()
        → Modifica configuración
      → MainWindow::updateCanvas()
```

## 4. PATRONES DE DISEÑO

| Patrón        | Uso                  | Archivos                 |
| ------------- | -------------------- | ------------------------ |
| **Observer**  | Eventos UI           | Todo wxWidgets           |
| **Strategy**  | Integradores         | `Integrators.hpp`        |
| **Factory**   | Campos vectoriales   | `VectorField.hpp`        |
| **Singleton** | Ventana principal    | `MainWindow` (implícito) |
| **MVC**       | Separación UI/Lógica | Toda la app              |
| **RAII**      | Gestión de recursos  | Todos los headers        |

## 5. FORMATO DE ARCHIVOS

### 5.1 Escenario (.scenario)

```json
{
  "version": 1,
  "name": "Río uniforme",
  "width": 50,
  "height": 50,
  "cells": [
    /* 0=agua, 1=tierra, 2=obstáculo */
  ],
  "field": {
    "type": "uniform",
    "params": { "intensity": 2.0, "angle": 0.0 }
  },
  "boat": {
    "position": { "x": 5.0, "y": 25.0 },
    "mass": 10.0,
    "size": 5.0
  }
}
```

### 5.2 Datos CSV

```csv
Tiempo(s),PosX,PosY,VelX,VelY,Velocidad(m/s),Energia(J)
0.033,5.100,25.000,3.000,0.000,3.000,45.000
...
```

## 6. RENDIMIENTO

### 6.1 Límites

- **Partículas**: 800 máximo (WaterParticleSystem)
- **Trayectoria**: Sin límite, pero solo últimos 1000 puntos renderizados
- **Celdas**: 200x200 máximo (Scenario)
- **FPS**: Target 30, configurable

### 6.2 Optimizaciones

- Viewport culling en partículas
- Grid-based rendering en escenario
- Lazy evaluation de trayectoria
- wxWidgets con doble buffer

## 7. EXTENSIBILIDAD

### 7.1 Agregar Nuevo Integrador

1. Heredar de `IIntegrator`
2. Implementar `integrate()`
3. Agregar a enum `IntegrationMethod`
4. Registrar en `SimulationRunner`
5. Agregar a UI (RightPanel)

### 7.2 Agregar Nuevo Campo Vectorial

1. Crear método factory en `VectorField`
2. Implementar función lambda
3. Agregar a UI (RightPanel::fieldTypeChoice)
4. Actualizar `onScenarioSelect()` si es escenario predefinido

### 7.3 Agregar Nuevo Escenario

1. Añadir a lista en `LeftPanel`
2. Implementar lógica en `onScenarioSelect()`
3. Actualizar documentación UX

## 8. DEPENDENCIAS

### 8.1 Bibliotecas

- **wxWidgets 3.3.2**: GUI
- **STL**: Contenedores, algoritmos
- **cmath**: Funciones matemáticas

### 8.2 Sistema

- Windows 7+ (Win32 API)
- MinGW-w64 o MSVC
- OpenGL (opcional, para futuras versiones)

## 9. CONVENIOS DE CÓDIGO

### 9.1 Nomenclatura

| Tipo       | Convención     | Ejemplo               |
| ---------- | -------------- | --------------------- |
| Clases     | PascalCase     | `SimulationRunner`    |
| Funciones  | camelCase      | `runSimulationStep()` |
| Variables  | camelCase + \_ | `currentTime_`        |
| Constantes | UPPER_SNAKE    | `MAX_PARTICLES`       |
| Enum       | PascalCase + e | `IntegrationMethod`   |

### 9.2 Comentarios

```cpp
/**
 * @brief Descripción breve
 * @param param Descripción parámetro
 * @return Descripción retorno
 * @throws Excepciones (si aplica)
 */
```

### 9.3 Headers

- `#pragma once` para guardas
- Orden includes: sistema, wxWidgets, proyecto
- Forward declarations cuando sea posible

## 10. PRUEBAS

### 10.1 Estrategia

- Tests unitarios: Capas Domain y Simulation
- Tests de integración: Flujos completos
- Tests manuales: UI/UX

### 10.2 Escenarios de Prueba

1. **Río uniforme**: Trayectoria lineal predecible
2. **Remolino**: Comportamiento espiral
3. **Colisión**: Respuesta correcta al impacto
4. **Estabilidad**: Energía conservada (RK4)

---

**Documento consolidado**: Reemplaza Architecture.md original
**Última actualización**: 2025-03-23
**Versión código**: 0.2.0
