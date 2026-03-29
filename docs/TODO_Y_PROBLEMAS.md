# TODO Y PROBLEMAS CONOCIDOS

## Ultima Actualizacion

**Fecha**: Marzo 2026
**Version**: 0.2.3
**Estado**: Funcional con mejoras implementadas (95% completado)

---

## FUNCIONALIDADES IMPLEMENTADAS (v0.2.2)

### 1. Empuje Automatico (Auto Thrust) - COMPLETADO ✅

**Archivos**: `src/presentation/main_window/MainWindow.cpp`, `src/presentation/main_window/MainWindow.hpp`

**Implementado**:

- Metodo `isAutoThrustEnabled()` en RightPanel (MainWindow.hpp:229)
- Logica en `runSimulationStep()` (MainWindow.cpp:1005-1016)
- Checkbox en UI del panel Motor
- Calcula direccion optima basada en el campo vectorial

**Codigo implementado**:

```cpp
// En RightPanel.hpp:
bool isAutoThrustEnabled() const { return autoThrustCheck_ ? autoThrustCheck_->GetValue() : false; }

// En MainWindow::runSimulationStep():
if (rightPanel_->isAutoThrustEnabled()) {
    Vec2d fieldVec = config.field.getValue(config.boat.getPosition().x,
                                            config.boat.getPosition().y);
    if (fieldVec.magnitude() > 0.1) {
        double fieldAngle = std::atan2(fieldVec.y, fieldVec.x);
        direction = fieldAngle;
    }
}
```

---

### 2. Integracion TheoryPanel -> Simulation - COMPLETADO ✅

**Archivos**: `src/presentation/panels/TheoryPanel.cpp`, `src/presentation/panels/TheoryPanel.hpp`, `src/presentation/main_window/MainWindow.cpp`

**Implementado**:

- Callback `scenarioCallback_` en TheoryPanel para notificar solicitud de escenario
- Metodo `onTheoryScenarioRequest()` en MainWindow que cambia al modo simulacion
- Selecciona automaticamente el escenario correspondiente

---

### 3. Sincronizacion ExperimentService - COMPLETADO ✅

**Archivos**: `src/application/ExperimentService.hpp`, `src/application/ExperimentService.cpp`, `src/presentation/main_window/MainWindow.cpp`

**Implementado**:

- `ScenarioPreset` struct con nombre y funcion de configuracion
- Lista de presets en ExperimentService (6 escenarios)
- `getScenarioPresetCount()`, `getScenarioPresetName()`, `applyScenarioPreset()`
- Lista de escenarios en LeftPanel se carga desde el servicio
- `updateScenarioList()` sincroniza con ExperimentService
- `onScenarioSelect()` usa `applyScenarioPreset()` en lugar de codigo duplicado

---

### 4. Pestana Escenario en RightPanel - COMPLETADO ✅

**Archivos**: `src/presentation/main_window/MainWindow.cpp`

**Implementado**:

- Nueva pestana "Escenario" en RightPanel
- Muestra dimensiones, conteo de celdas (agua, tierra, obstaculos)
- Informacion sobre usar el Editor (F3)

---

### 5. Sistema Undo/Redo - COMPLETADO ✅

**Archivos**: `src/presentation/panels/EditorCommand.hpp`, `src/presentation/panels/EditorPanel.cpp`

**Implementado**:

- Clase `CommandHistory` con pila de comandos (max 50)
- Comandos implementados:
  - `PaintCellCommand` - Pintar celda individual
  - `PaintStrokeCommand` - Pintar trazo (múltiples celdas)
  - `ClearScenarioCommand` - Limpiar escenario completo
  - `FillScenarioCommand` - Llenar con tipo específico
- Handlers `onUndo()` y `onRedo()` en EditorPanel
- Integracion con `commandHistory_.executeCommand()` para operaciones de editor

**Codigo implementado**:

```cpp
// En EditorPanel::onUndo():
void EditorPanel::onUndo(wxCommandEvent& event) {
    (void)event;
    if (commandHistory_.canUndo()) {
        commandHistory_.undo();
        if (mainWindow_) {
            mainWindow_->updateCanvas();
        }
    }
}
```

---

### 6. EditorPanel - TODOs 007-013 COMPLETADOS ✅

**Archivos**: `src/presentation/panels/EditorPanel.cpp`

**Implementado**:

- **TODO-007**: `setCurrentTool()` actualiza colores de botones (fondo + texto) + Refresh()
- **TODO-008**: `onBrushSizeChange()` llama a `mainWindow_->updateEditorBrushSize()`
- **TODO-009**: `onSnapToggle()` llama a `mainWindow_->updateEditorSnap()`
- **TODO-010**: `onNewScenario()` crea escenario con dimensiones especificadas
- **TODO-011**: `onResizeScenario()` redimensiona manteniendo contenido proporcional
- **TODO-012**: `onFieldTypeChange()` cambia tipo de campo (Uniforme, Lineal, Radial, Rotacional)
- **TODO-013**: `onFieldParamChange()` y `onSpinCtrlChange()` actualizan intensidad y centro

---

### 7. UI Sliders - TODOs 001-003 COMPLETADOS ✅

**Archivos**: `src/presentation/main_window/MainWindow.cpp`, `src/presentation/main_window/RightPanel.cpp`

**Implementado**:

- `getThrustValue()` - Retorna valor del slider de empuje
- `getDirectionValue()` - Retorna valor del slider de dirección
- `getTimeStep()` - Retorna valor del slider de paso temporal
- Utilizados en `runSimulationStep()` para configurar la simulación

---

### 8. Escenarios Predefinidos - 10 escenarios ✅

**Archivos**: `src/application/ExperimentService.cpp`

**Implementado**: 10 escenarios predefinidos (↑4 desde v0.2.1)

1. Río uniforme
2. Remolino central
3. Canal con obstáculo
4. Zona turbulenta
5. Laberinto de canales
6. Zona de rápidos
7. Efecto Venturi (nuevo)
8. Puerto con rompeolas (nuevo)
9. Corrientes en capas (nuevo)
10. Corriente oscilatoria (nuevo)

---

### 9. Sistema de Notificaciones Observer ✅

**Archivos**: `src/application/ExperimentService.hpp`, `src/application/ExperimentService.cpp`

**Implementado**:

- addConfigChangeListener() para registrar callbacks
- notifyConfigChange() llamado en todos los setters (setScenario, setField, setBoat, setMotor)
- RightPanel::updateProperties() sincroniza automáticamente la UI
- Integración completa con MainWindow

**Handlers con notificación**: Todos los handlers de RightPanel ahora llaman notifyConfigChange:

- updateBoatFromUI()
- updateMotorFromUI()
- updateFieldFromUI()
- updateNumericalFromUI()

---

### 10. Sistema de Partículas (Estela/Espuma/Splash) ✅

**Archivos**: `src/presentation/widgets/WaterParticleSystem.hpp/cpp`, `src/presentation/main_window/MainWindow.cpp`

**Implementado**:

- WaterParticleSystem existente y funcional
- Integración en runSimulationStep():
  - emitWake() - estela detrás del bote basada en velocidad
  - emitFoam() - espuma alrededor del bote
  - applyFieldEffect() - efecto de campo vectorial
  - emitSplash() - salpicaduras en colisiones
- update(dt) llamado cada paso de simulación

---

### 11. Métricas Avanzadas (Desplazamiento/Eficiencia) ✅

**Archivos**: `src/presentation/main_window/MainWindow.hpp/cpp`

**Implementado**:

- netDisplacement\_ - desplazamiento neto (línea recta desde el inicio)
- routeEfficiency\_ - eficiencia de ruta (desplazamiento / distancia total)
- Getters: getNetDisplacement(), getRouteEfficiency()
- Display en BottomPanel: nuevos textos para desplazamiento y eficiencia
- Cálculo automático en cada paso de simulación

---

### 12. Auditoría de Handlers ✅

**Archivo**: `docs/HANDLERS_AUDIT.md`

**Resultado**: 59 handlers implementados (100%)

| Componente       | Handlers |
| ---------------- | -------- |
| MainWindow       | 29       |
| LeftPanel        | 3        |
| RightPanel       | 6        |
| EditorPanel      | 13       |
| TheoryPanel      | 6        |
| SimulationCanvas | 5        |
| **TOTAL**        | **59**   |

---

### 13. Tests Unitarios ✅

**Directorio**: `tests/`

**Tests implementados**:

- test_boat.cpp
- test_vector_field.cpp
- test_scenario.cpp
- test_motor.cpp (nuevo)
- test_integrators.cpp
- test_physics.cpp
- test_experiment_service.cpp

**Coverage**: ~40% del dominio y simulación

---

## FUNCIONALIDADES PENDIENTES

### 1. Campo Vectorial Personalizado (Expresiones Matemáticas) - COMPLETADO ✅

**Archivos**: `src/domain/MathExpression.hpp`, `src/presentation/main_window/MainWindow.cpp`

**Implementado**:

- Parser de expresiones matemÃ¡ticas completo en MathExpression.hpp
- Soporte para funciones: sin, cos, tan, sqrt, abs, exp, log
- Soporte para variables x, y
- Constantes: pi, e
- Operadores: +, -, \*, /, ^
- Dialogo en onCustomField usa el parser correctamente

---

### 2. Atajos de Teclado (Ctrl+Z / Ctrl+Y) - COMPLETADO ✅

**Archivos**: `src/presentation/panels/EditorPanel.cpp`

**Implementado**:

- wxAcceleratorTable con Ctrl+Z para undo y Ctrl+Y para redo
- Agregado en el constructor de EditorPanel

**Codigo implementado**:

```cpp
wxAcceleratorEntry entries[2];
entries[0].Set(wxACCEL_CTRL, (int)'Z', ID_UNDO);
entries[1].Set(wxACCEL_CTRL, (int)'Y', ID_REDO);
wxAcceleratorTable accelTable(2, entries);
SetAcceleratorTable(accelTable);
```

---

### 3. Sincronizacion con ExperimentService (Patron Observer) - COMPLETADO ✅

**Archivos**: `src/application/ExperimentService.hpp`, `src/application/ExperimentService.cpp`, `src/presentation/main_window/MainWindow.cpp`

**Implementado**:

- addConfigChangeListener() en ExperimentService
- notifyConfigChange() llamado en setters (setScenario, setField, setBoat, setMotor)
- RightPanel::updateProperties() ahora sincroniza valores desde ExperimentService
- Callback registrado en MainWindow para actualizar UI automaticamente

---

## PROBLEMAS CONOCIDOS

### 1. Botones de Ventana No Visibles

**Severidad**: Media
**Descripcion**: La ventana no muestra los botones minimizar, maximizar y cerrar (X) en la esquina superior derecha
**Causa**: Posiblemente un bug de wxWidgets AUI que interfere con los estilos de ventana nativos
**Workaround**: Usar Alt+F4 para cerrar, o clic derecho en barra de tareas
**Estado**: NO RESUELTO - Requiere investigacion profunda de wxWidgets AUI

### 2. Errores LSP/Headers

**Severidad**: Baja (solo en desarrollo)
**Descripcion**: El LSP reporta errores de include de 'Vec2.hpp' y tipos no encontrados
**Causa**: Problemas de configuracion del language server, no del codigo
**Nota**: El codigo compila correctamente, son falsos positivos del LSP

### 3. Compilacion Manual Requerida

**Severidad**: Media
**Descripcion**: No hay CI/CD automatico configurado
**Solucion**: Requiere acceso a MSYS2/MinGW o Visual Studio para compilar
**Nota**: El ejecutable en /build/bin/ puede estar desactualizado

---

## MEJORAS SUGERIDAS (FUTURO)

### Alto Impacto / Bajo Esfuerzo

1. **Mas escenarios**: Agregar 3-5 escenarios mas (puente, cascada, islas)
2. **Colores personalizables**: Permitir cambiar colores del agua, tierra, bote
3. **Guardar configuracion**: Guardar preferencias de usuario en archivo INI

### Alto Impacto / Alto Esfuerzo

1. **Soporte 3D**: Usar OpenGL para visualizacion 3D del terreno
2. **Multiplayer**: Permitir 2+ botes simultaneos
3. **Scripting**: Integrar Lua o Python para scripts personalizados
4. **Video export**: Grabar simulacion como video MP4

### Bajo Impacto / Bajo Esfuerzo

1. **Tooltips mejorados**: Agregar tooltips explicativos a todos los controles
2. **Sonidos**: Efectos de sonido para colisiones y eventos
3. **Iconos**: Mejorar iconos de toolbar con set profesional

---

## ARCHIVOS CLAVE PARA CONTINUAR

### Documentacion

- `docs/ARCHITECTURE_CONSOLIDATED.md` - Arquitectura del sistema
- `docs/BUTTON_VERIFICATION.md` - Verificacion de todos los botones
- `docs/product_spec_proyecto_cpp_taller_parametrico.md` - Especificacion funcional
- `docs/ux_and_screens_proyecto_cpp_taller_parametrico.md` - Especificacion UX
- `docs/ROADMAP.md` - Roadmap de implementacion

### Codigo Fuente Principal

- `src/presentation/main_window/MainWindow.cpp` - Ventana principal y logica
- `src/presentation/panels/EditorPanel.cpp` - Panel de edicion (falta undo/redo)
- `src/presentation/widgets/SimulationCanvas.cpp` - Canvas de simulacion
- `src/domain/Boat.cpp` - Fisica del bote
- `src/simulation/integrators/` - Metodos numericos
- `src/application/ExperimentService.cpp` - Servicio de experimentos

### Build

- `CMakeLists.txt` - Configuracion principal CMake
- `CMakePresets.json` - Presets de compilacion

---

## COMANDOS UTILES

### Compilar desde MSYS2

```bash
cd build
cmake .. -G Ninja
cmake --build . --target TallerParametrico
```

### Compilar desde VS

```bash
cd build
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" .. -G Ninja
cmake --build . --target TallerParametrico
```

---

## DEPENDENCIAS EXTERNAS

### Requeridas para Compilar

- wxWidgets 3.3.2 (ya incluido en build/wx_installed/)
- CMake 3.20+
- Ninja o Make
- MinGW-w64 o MSVC 2022

### Requeridas para Ejecutar

- Windows 7/10/11 (64-bit)
- Visual C++ Redistributable 2015-2022
- DLLs de MinGW (libgcc, libstdc++, pthread) - ya copiadas en bin/

---

## NOTAS PARA DESARROLLADORES FUTUROS

1. **No usar `concept` como nombre de variable**: Es palabra reservada de C++20
2. **Incluir `<cmath>` para M_PI**: Algunos compiladores no lo definen por defecto
3. **Definir ssize_t**: Windows no lo tiene, usar `#define ssize_t long`
4. **Orden de librerias wxWidgets**: Las dependientes van primero (aui, html, adv, core)
5. **Estilos de ventana**: Si se arregla el problema de la X, probar wxDEFAULT_FRAME_STYLE sin modificaciones
6. **ExperimentService**: Ahora es la fuente unica de verdad para los escenarios predefinidos

---

## PENDIENTE - Por Implementar

### Fase 1: Estabilización Crítica

âœ… **TODOs 001-005**: Completados
âœ… **TODOs 006-015**: Completados

### Fase 2: Funcionalidades Core

- Parser de expresiones: ✅ Ya existe (MathExpression.hpp)
- Atajos de teclado: ✅ Implementados (Ctrl+Z, Ctrl+Y)
- Sincronizacion Observer: ✅ Implementada

**No hay tareas pendientes en Fase 1**

---

**Documento creado para**: Continuacion del proyecto por otra IA o desarrollador  
**Contacto**: Ver README.md para enlaces de soporte
