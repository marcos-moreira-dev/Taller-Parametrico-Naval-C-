# Auditoría de Handlers del Sistema

**Fecha**: Marzo 2026  
**Versión**: 0.2.3

---

## Resumen General

| Componente       | Handlers | Estado                                |
| ---------------- | -------- | ------------------------------------- |
| MainWindow       | 26       | ✅ Completos                          |
| LeftPanel        | 3        | ✅ Completos                          |
| RightPanel       | 6        | ✅ Completos (con notifyConfigChange) |
| EditorPanel      | 13       | ✅ Completos                          |
| TheoryPanel      | 6        | ✅ Completos                          |
| SimulationCanvas | 5        | ✅ Completos                          |
| **TOTAL**        | **59**   | **100%** ✅                           |

---

## Detalle por Componente

### MainWindow (26 handlers)

| Handler                 | Estado | Descripción                            |
| ----------------------- | ------ | -------------------------------------- |
| onNew                   | ✅     | Nuevo experimento                      |
| onOpen                  | ✅     | Abrir archivo                          |
| onSave                  | ✅     | Guardar archivo                        |
| onExportImage           | ✅     | Exportar imagen                        |
| onExportData            | ✅     | Exportar datos CSV                     |
| onExit                  | ✅     | Salir de aplicación                    |
| onViewLeftPanel         | ✅     | Mostrar/ocultar panel izquierdo        |
| onViewRightPanel        | ✅     | Mostrar/ocultar panel derecho          |
| onViewBottomPanel       | ✅     | Mostrar/ocultar panel inferior         |
| onViewGrid              | ✅     | Mostrar/ocultar grid                   |
| onViewField             | ✅     | Mostrar/ocultar campo vectorial        |
| onViewTrajectory        | ✅     | Mostrar/ocultar trayectoria            |
| onModeTheory            | ✅     | Cambiar a modo Teoría                  |
| onModeSimulation        | ✅     | Cambiar a modo Simulación              |
| onModeEditor            | ✅     | Cambiar a modo Editor                  |
| onTheoryScenarioRequest | ✅     | Request de escenario desde TheoryPanel |
| onStartSimulation       | ✅     | Iniciar simulación                     |
| onPauseSimulation       | ✅     | Pausar simulación                      |
| onResetSimulation       | ✅     | Reiniciar simulación                   |
| onStepSimulation        | ✅     | Un paso de simulación                  |
| onMethodChange          | ✅     | Cambiar método numérico                |
| onDtChange              | ✅     | Cambiar paso temporal                  |
| onAbout                 | ✅     | MostrarAcerca de                       |
| onHelp                  | ✅     | Mostrar ayuda                          |
| onResetView             | ✅     | Restablecer vista                      |
| onClearTrajectory       | ✅     | Limpiar trayectoria                    |
| onCaptureScreen         | ✅     | Capturar pantalla                      |
| onCustomField           | ✅     | Campo vectorial personalizado          |
| onTimer                 | ✅     | Timer de simulación                    |

---

### LeftPanel (3 handlers)

| Handler          | Estado | Descripción                             |
| ---------------- | ------ | --------------------------------------- |
| onScenarioSelect | ✅     | Seleccionar escenario de la lista       |
| onModeSelect     | ✅     | Cambiar modo (Teoría/Simulación/Editor) |
| onLayerToggle    | ✅     | Alternar visibilidad de capas           |

---

### RightPanel (6 handlers)

| Handler                | Estado | Descripción                            | Notifica? |
| ---------------------- | ------ | -------------------------------------- | --------- |
| onBoatParamChange      | ✅     | Cambiar parámetros del bote (SpinCtrl) | ✅        |
| onMotorParamChange     | ✅     | Cambiar empuje/dirección (Slider)      | ✅        |
| onFieldTypeChange      | ✅     | Cambiar tipo de campo (Choice)         | ✅        |
| onFieldParamChange     | ✅     | Cambiar intensidad del campo (Slider)  | ✅        |
| onNumericalParamChange | ✅     | Cambiar método/dt (Choice/Slider)      | ✅        |
| onDurationChange       | ✅     | Cambiar duración máxima (SpinCtrl)     | ✅        |

**Nota**: Todos los handlers de RightPanel ahora llaman a `notifyConfigChange()` para sincronización automática con la UI.

---

### EditorPanel (13 handlers)

| Handler            | Estado | Descripción                                                     |
| ------------------ | ------ | --------------------------------------------------------------- |
| onToolSelect       | ✅     | Seleccionar herramienta (Select/Water/Land/Obstacle/Erase/Boat) |
| onBrushSizeChange  | ✅     | Cambiar tamaño del brush (Slider)                               |
| onSnapToggle       | ✅     | Activar/desactivar snap to grid (Checkbox)                      |
| onNewScenario      | ✅     | Crear nuevo escenario                                           |
| onResizeScenario   | ✅     | Redimensionar escenario                                         |
| onClearScenario    | ✅     | Limpiar escenario (con undo)                                    |
| onFillWater        | ✅     | Llenar con agua (con undo)                                      |
| onFillLand         | ✅     | Llenar con tierra (con undo)                                    |
| onUndo             | ✅     | Deshacer (Ctrl+Z)                                               |
| onRedo             | ✅     | Rehacer (Ctrl+Y)                                                |
| onFieldTypeChange  | ✅     | Cambiar tipo de campo en editor                                 |
| onFieldParamChange | ✅     | Cambiar parámetros del campo                                    |
| onSpinCtrlChange   | ✅     | Cambiar centro del campo (X, Y)                                 |

---

### TheoryPanel (6 handlers)

| Handler          | Estado | Descripción                   |
| ---------------- | ------ | ----------------------------- |
| onConceptSelect  | ✅     | Seleccionar concepto teórico  |
| onCategorySelect | ✅     | Filtrar por categoría         |
| onNext           | ✅     | Siguiente concepto            |
| onPrevious       | ✅     | Concepto anterior             |
| onOpenScenario   | ✅     | Abrir escenario en simulación |
| onSearch         | ✅     | Buscar conceptos              |

---

### SimulationCanvas (5 handlers)

| Handler    | Estado | Descripción            |
| ---------- | ------ | ---------------------- |
| OnPaint    | ✅     | Renderizar canvas      |
| OnSize     | ✅     | Ajustar tamaño         |
| OnLeftDown | ✅     | Clic para pintar/mover |
| OnMotion   | ✅     | Movimiento del mouse   |
| OnLeftUp   | ✅     | Soltar mouse           |

---

## Notas

- Todos los 59 handlers del sistema están implementados
- Los handlers de RightPanel fueron mejorados para llamar `notifyConfigChange()`
- Los atajos de teclado Ctrl+Z/Ctrl+Y funcionan en EditorPanel
- No hay handlers pendientes de implementación

---

**Última actualización**: Marzo 2026
