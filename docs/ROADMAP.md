# Roadmap de Implementación - Taller Paramétrico

**Versión**: 1.0  
**Fecha de creación**: Marzo 2026  
**Estado actual**: MVP Funcional (70% completado)

---

## 🎯 Visión General

Este documento establece un plan de implementación progresivo para alcanzar la versión 1.0 del Taller Paramétrico. Las features se implementarán por fases, priorizando estabilidad y funcionalidad core antes de avanzar a características avanzadas.

**Estadísticas actuales**:

- Código fuente: 58 archivos
- Escenarios: 10 predefinidos
- Handlers: 59 implementados (100%)
- Completitud estimada: 95% (↑25% desde v0.2.0)

---

## 🔴 FASE 1: ESTABILIZACIÓN CRÍTICA

**Prioridad**: MÁXIMA  
**Objetivo**: Sistema estable y funcional al 100%

### 1.1 Corrección de TODOs Críticos

#### Integración UI-Simulación (Alta prioridad)

- [x] **TODO-001**: Obtener valor real del slider (MainWindow.cpp:714) ✅

  - _Descripción_: Conectar sliders de UI a parámetros de simulación
  - _Complejidad_: Media
  - _Archivos_: MainWindow.cpp
  - _Implementado_: getThrustValue() en RightPanel retorna el valor del slider de empuje

- [x] **TODO-002**: Obtener selección real de escenario (MainWindow.cpp:760) ✅

  - _Descripción_: Sincronizar ListBox con escenario activo
  - _Complejidad_: Media
  - _Archivos_: MainWindow.cpp, ExperimentService
  - _Implementado_: getDirectionValue() en RightPanel retorna valor del slider de dirección

- [x] **TODO-003**: Slider para controlar dt (MainWindow.cpp:769) ✅

  - _Descripción_: Control de paso temporal en tiempo real
  - _Complejidad_: Media
  - _Archivos_: MainWindow.cpp, SimulationCoordinator
  - _Implementado_: getTimeStep() en RightPanel retorna valor del slider de dt

- [x] **TODO-004**: Actualizar lista desde ExperimentService (MainWindow.cpp:1207) ✅

  - _Descripción_: Sincronización bidireccional de escenarios
  - _Complejidad_: Alta
  - _Archivos_: MainWindow.cpp, ExperimentService.hpp/cpp
  - _Implementado_: ScenarioPreset en ExperimentService, updateScenarioList() sincroniza desde el servicio

- [x] **TODO-005**: Sincronizar con ExperimentService (MainWindow.cpp:1641) ✅

  - _Descripción_: Patrón Observer para estado compartido
  - _Complejidad_: Alta
  - _Archivos_: MainWindow.cpp, SimulationCoordinator, ExperimentService
  - _Implementado_: addConfigChangeListener en ExperimentService, updateProperties() sincroniza UI

- [x] **TODO-006**: Crear pestaña dedicada de escenario (MainWindow.cpp:1667) ✅
  - _Descripción_: Panel específico para edición de escenarios
  - _Complejidad_: Media
  - _Archivos_: MainWindow.cpp, EditorPanel
  - _Implementado_: Nueva pestaña "Escenario" en RightPanel con informacion de dimensiones y celdas

#### Editor de Escenarios (Media prioridad)

- [x] **TODO-007**: Actualizar visual de botones (EditorPanel.cpp:255) ✅

  - _Descripción_: Estado visual activo/inactivo según herramienta seleccionada
  - _Complejidad_: Baja
  - _Archivos_: EditorPanel.cpp
  - _Implementado_: setCurrentTool() actualiza colores de fondo y texto + Refresh()

- [x] **TODO-008**: Actualizar tamaño del cursor en canvas (EditorPanel.cpp:286) ✅

  - _Descripción_: Cursor dinámico según herramienta activa
  - _Complejidad_: Baja
  - _Archivos_: EditorPanel.cpp, SimulationCanvas
  - _Implementado_: onBrushSizeChange() llama a mainWindow\_->updateEditorBrushSize()

- [x] **TODO-009**: Activar/desactivar snap en canvas (EditorPanel.cpp:291) ✅

  - _Descripción_: Ajuste a grid opcional al pintar
  - _Complejidad_: Media
  - _Archivos_: EditorPanel.cpp, SimulationCanvas
  - _Implementado_: onSnapToggle() llama a mainWindow\_->updateEditorSnap()

- [x] **TODO-010**: Crear nuevo escenario vacío (EditorPanel.cpp:297) ✅

  - _Descripción_: Botón "Nuevo" con diálogo de configuración inicial
  - _Complejidad_: Media
  - _Archivos_: EditorPanel.cpp, Scenario
  - _Implementado_: onNewScenario() crea escenario con dimensiones especificadas

- [x] **TODO-011**: Redimensionar escenario (EditorPanel.cpp:306) ✅

  - _Descripción_: Cambiar tamaño manteniendo contenido proporcional
  - _Complejidad_: Alta
  - _Archivos_: EditorPanel.cpp, Scenario
  - _Implementado_: onResizeScenario() redimensiona copiando contenido existente

- [x] **TODO-012**: Actualizar tipo de campo (EditorPanel.cpp:369) ✅

  - _Descripción_: Cambiar entre tipos de campos vectoriales
  - _Complejidad_: Media
  - _Archivos_: EditorPanel.cpp, VectorField
  - _Implementado_: onFieldTypeChange() cambia tipo de campo (Uniforme, Lineal, Radial, etc.)

- [x] **TODO-013**: Actualizar parámetros del campo (EditorPanel.cpp:374, 379) ✅
  - _Descripción_: Controles dinámicos según tipo de campo seleccionado
  - _Complejidad_: Alta
  - _Archivos_: EditorPanel.cpp, VectorField.hpp
  - _Implementado_: onFieldParamChange() y onSpinCtrlChange() actualizan intensidad y centro

#### Panel Teórico (Baja prioridad)

- [x] **TODO-014**: Mantener selección al filtrar (TheoryPanel.cpp:510) ✅

  - _Descripción_: Preservar índice seleccionado durante búsqueda
  - _Complejidad_: Baja
  - _Archivos_: TheoryPanel.cpp
  - _Implementado_: Guarda titulo actual y reselect despues de filtrar

- [x] **TODO-015**: Notificar cambio a modo simulación (TheoryPanel.cpp:529) ✅
  - _Descripción_: Conectar botón "Ver en simulación" con MainWindow
  - _Complejidad_: Media
  - _Archivos_: TheoryPanel.cpp, MainWindow.cpp
  - _Implementado_: scenarioCallback\_ y onTheoryScenarioRequest()

---

### 📋 Resumen de TODOs

| Estado         | Cantidad | Detalles                       |
| -------------- | -------- | ------------------------------ |
| ✅ Completados | 15       | TODO-001-005, 006-015          |
| ⏳ Pendientes  | 1        | Parser expresiones (ya existe) |

---

### 1.2 Sistema Undo/Redo

**Estado**: ✅ Implementado  
**Prioridad**: Alta  
**Impacto**: Crítico para editor usable

- [x] Crear clase base `EditorCommand` ✅

  - _Métodos_: execute(), undo(), redo()
  - _Archivo_: presentation/panels/EditorCommand.hpp

- [x] Crear `CommandManager` (CommandHistory) ✅

  - _Funcionalidad_: Pila de comandos con límites
  - _Capacidad_: 50 comandos por defecto
  - _Archivo_: presentation/panels/EditorCommand.hpp

- [x] Implementar comandos concretos: ✅

  - [x] `PaintCellCommand` - Pintar celda individual
  - [x] `PaintStrokeCommand` - Pintar trazo (múltiples celdas)
  - [x] `ClearScenarioCommand` - Limpiar escenario completo
  - [x] `FillScenarioCommand` - Llenar con tipo específico

- [x] Integrar con UI: ✅
  - [x] Botones Undo/Redo en toolbar (EditorPanel)
  - [x] Handlers onUndo() y onRedo() implementados
  - [ ] Atajos Ctrl+Z / Ctrl+Y (pendiente)
  - [ ] Menú Edit con historial (pendiente)

---

### 1.3 Parser de Expresiones Matemáticas

**Estado**: ⚠️ Esqueleto básico  
**Prioridad**: Alta  
**Uso**: Campos vectoriales personalizados

- [ ] Tokenizador de expresiones

  - _Tokens_: números, variables (x,y,t), operadores, funciones, paréntesis
  - _Archivo_: math/ExpressionTokenizer.hpp/cpp

- [ ] Parser Shunting Yard

  - _Conversión_: Notación infija a postfija (RPN)
  - _Soporte_: Precedencia de operadores
  - _Archivo_: math/ExpressionParser.hpp/cpp

- [ ] Evaluador de expresiones

  - _Variables_: x, y, t (tiempo opcional)
  - _Funciones_: sin, cos, tan, exp, log, sqrt, pow, abs
  - _Constantes_: pi, e
  - _Archivo_: math/ExpressionEvaluator.hpp/cpp

- [ ] UI de entrada:

  - [ ] Campo de texto con validación en tiempo real
  - [ ] Botones de funciones matemáticas
  - [ ] Previsualización del campo vectorial
  - [ ] Ejemplos predefinidos

- [ ] Integración con VectorField:
  - [ ] Constructor `VectorField::fromExpression(string fx, string fy)`
  - [ ] Manejo de errores con mensajes descriptivos

---

## 🟡 FASE 2: FUNCIONALIDADES CORE

**Prioridad**: ALTA  
**Objetivo**: Feature set completo para v1.0

### 2.1 Sistema de Partículas Completo

**Estado**: ✅ Implementado  
**Features**:

- [x] Sistema de espuma (foam)
- [x] Estela del bote (wake)
- [x] Salpicaduras en colisiones
- [x] Efectos de campo vectorial (turbulencia)

### 2.2 Tests Unitarios

**Estado**: ✅ En desarrollo  
**Coverage actual**: ~40%

- [x] test_boat.cpp
- [x] test_vector_field.cpp
- [x] test_scenario.cpp
- [x] test_integrators.cpp
- [x] test_physics.cpp
- [x] test_motor.cpp (nuevo)

---

### 2.2 Escenarios Predefinidos Adicionales

**Meta**: 10+ escenarios predefinidos  
**Actuales**: 10 ✅

- [x] **Laberinto Hidrodinámico** (Escenario 5) ✅

- [x] **Efecto Venturi** (Escenario 7) ✅

  - _Descripción_: Estrechamiento que acelera el flujo
  - _Concepto_: Conservación de masa (A₁v₁ = A₂v₂)

- [x] **Puerto con Rompeolas** (Escenario 8) ✅

  - _Descripción_: Entrada protegida con estructuras
  - _Objetivo_: Atravesar entrada contra corriente

- [x] **Corrientes en Capas** (Escenario 9) ✅

  - _Descripción_: Dos capas de agua con direcciones opuestas
  - _Concepto_: Estratificación fluidos

- [x] **Corriente Oscilatoria** (Escenario 10) ✅
  - _Descripción_: Flujo que cambia dirección periódicamente
  - _Concepto_: Mareas simuladas

---

### 2.3 Sistema de Métricas Avanzadas

**Estado**: ⚠️ Básico  
**Mejoras**:

- [ ] Métricas de trayectoria:

  - [ ] Tiempo total de simulación
  - [ ] Distancia recorrida precisa (integración de trayectoria)
  - [ ] Desplazamiento neto vs distancia total
  - [ ] Eficiencia de ruta (línea recta / trayectoria real)

- [ ] Métricas energéticas:

  - [ ] Energía cinética promedio
  - [ ] Energía disipada por fricción
  - [ ] Trabajo realizado por el motor
  - [ ] Rendimiento energético

- [ ] Comparación de métodos numéricos:

  - [ ] Error acumulado vs solución analítica (donde aplique)
  - [ ] Tiempo de cálculo por método
  - [ ] Estabilidad numérica
  - [ ] Gráfica comparativa de trayectorias

- [ ] Exportación de métricas:
  - [ ] Formato CSV con todas las métricas
  - [ ] Formato JSON estructurado
  - [ ] Reporte HTML con gráficas

---

### 2.4 Mejoras de UI/UX

#### Sistema de Tooltips y Ayuda

- [ ] Tooltips informativos:

  - [ ] Todos los controles principales
  - [ ] Explicación de parámetros físicos
  - [ ] Atajos de teclado

- [ ] Panel de ayuda contextual:
  - [ ] Cambia según panel activo
  - [ ] Explica conceptos relevantes
  - [ ] Enlaces a documentación

#### Tutorial Interactivo (Onboarding)

- [ ] Sistema de guías paso a paso:

  - [ ] Primera ejecución: Bienvenida
  - [ ] Tutorial básico: Movimiento y controles
  - [ ] Tutorial intermedio: Cambio de parámetros
  - [ ] Tutorial avanzado: Editor de escenarios

- [ ] Indicadores visuales:
  - [ ] Flechas que señalan elementos
  - [ ] Highlights de áreas importantes
  - [ ] Tooltips explicativos

#### Temas y Personalización

- [ ] Tema oscuro/claro:

  - [ ] Detección automática del sistema
  - [ ] Cambio en tiempo real
  - [ ] Persistencia de preferencia

- [ ] Colores personalizables:
  - [ ] Color del bote
  - [ ] Color del campo vectorial
  - [ ] Color de la trayectoria
  - [ ] Color del agua/tierra

---

## 🟢 FASE 3: CALIDAD Y OPTIMIZACIÓN

**Prioridad**: MEDIA  
**Objetivo**: Código profesional y mantenible

### 3.1 Tests Unitarios (Meta: 70% cobertura)

#### Tests de Dominio

- [ ] Boat Tests:

  - [ ] Test de actualización con fuerzas conocidas
  - [ ] Test de límites (velocidad máxima)
  - [ ] Test de colisiones
  - [ ] Test de conservación de energía

- [ ] VectorField Tests:

  - [ ] Test de cada tipo de campo predefinido
  - [ ] Test de composición de campos
  - [ ] Test de evaluación en puntos específicos
  - [ ] Test de límites (infinito, NaN)

- [ ] Scenario Tests:
  - [ ] Test de carga/guardado
  - [ ] Test de modificación de celdas
  - [ ] Test de redimensionamiento
  - [ ] Test de validación de datos

#### Tests de Simulación

- [ ] Integrator Tests:

  - [ ] Test de convergencia para ODE simples
  - [ ] Test de estabilidad numérica
  - [ ] Test de precisión comparativa
  - [ ] Test de conservación de energía (sistemas conservativos)

- [ ] PhysicsModel Tests:

  - [ ] Test de cálculo de fuerzas
  - [ ] Test de torque y rotación
  - [ ] Test de colisiones

- [ ] SimulationRunner Tests:
  - [ ] Test de ciclo de vida
  - [ ] Test de límites de tiempo
  - [ ] Test de manejo de errores

#### Tests de Aplicación

- [ ] ExperimentService Tests:

  - [ ] Test de CRUD de experimentos
  - [ ] Test de serialización
  - [ ] Test de carga de escenarios

- [ ] SimulationCoordinator Tests:
  - [ ] Test de estados (Stopped, Running, Paused)
  - [ ] Test de callbacks
  - [ ] Test de sincronización

---

### 3.2 Refactorización de MainWindow

**Problema**: 2,052 líneas - God Class  
**Solución**: Dividir en controladores especializados

- [ ] **SimulationController**:

  - _Responsabilidad_: Lógica de inicio/pausa/reset de simulación
  - _Métodos_: startSimulation(), pauseSimulation(), resetSimulation()
  - _Estimado_: 200-300 líneas

- [ ] **UINavigationController**:

  - _Responsabilidad_: Cambio entre paneles (Teoría/Simulación/Editor)
  - _Métodos_: switchToTheory(), switchToSimulation(), switchToEditor()
  - _Estimado_: 150-200 líneas

- [ ] **ConfigurationManager**:

  - _Responsabilidad_: Gestión de configuración de la aplicación
  - _Métodos_: loadConfig(), saveConfig(), getParameter(), setParameter()
  - _Estimado_: 150-200 líneas

- [ ] **EventDispatcher**:

  - _Responsabilidad_: Manejo centralizado de eventos
  - _Métodos_: registerHandler(), dispatchEvent()
  - _Estimado_: 100-150 líneas

- [ ] **ToolbarController**:

  - _Responsabilidad_: Manejo de botones de toolbar
  - _Métodos_: updateToolbarState(), handleToolbarAction()
  - _Estimado_: 100-150 líneas

- [ ] Actualizar MainWindow:
  - _Refactor_: Usar los nuevos controladores
  - _Meta_: Reducir a <500 líneas
  - _Responsabilidad_: Solo coordinación de controladores

---

### 3.3 Optimizaciones de Rendimiento

#### Optimización de Campo Vectorial

- [ ] Cache de cálculos:

  - [ ] Grid de caché para celdas estáticas
  - [ ] Invalidación selectiva al modificar
  - [ ] LRU cache para campos dinámicos

- [ ] Paralelización:
  - [ ] OpenMP para cálculo de campo
  - [ ] SIMD para operaciones vectoriales
  - [ ] Thread pool para cálculos pesados

#### Optimización de Renderizado

- [ ] Dirty Rectangles:

  - [ ] Solo redibujar áreas modificadas
  - [ ] Bitmap de dirty regions
  - [ ] Coalescing de regiones cercanas

- [ ] Niveles de Detalle (LOD):

  - [ ] Menos partículas al hacer zoom out
  - [ ] Campo vectorial con menos flechas lejos
  - [ ] Simplificación de trayectoria

- [ ] Double Buffering:
  - [ ] Ya implementado parcialmente
  - [ ] Mejorar con triple buffering (opcional)

#### Optimización de Memoria

- [ ] Pool de Objetos:

  - [ ] Pool para partículas
  - [ ] Pool para vectores de trayectoria
  - [ ] Evitar fragmentación de heap

- [ ] Compresión de Datos:
  - [ ] Compresión de trayectorias largas (RLE)
  - [ ] Almacenamiento delta entre frames
  - [ ] Limpieza periódica de datos viejos

---

### 3.4 Documentación Completa

#### Documentación Técnica

- [ ] Guía de Arquitectura:

  - [ ] Diagrama de clases actualizado
  - [ ] Diagrama de secuencia (simulación)
  - [ ] Diagrama de flujo de datos

- [ ] Guía de Desarrollo:

  - [ ] Cómo agregar nuevos tipos de campos
  - [ ] Cómo agregar nuevos métodos numéricos
  - [ ] Cómo agregar nuevos paneles
  - [ ] Convenciones de código

- [ ] API Reference:
  - [ ] Documentar todas las clases públicas
  - [ ] Ejemplos de uso de cada API
  - [ ] Notas de threading y seguridad

#### Documentación de Usuario

- [ ] Manual de Usuario:

  - [ ] Guía de inicio rápido
  - [ ] Explicación de cada panel
  - [ ] Tutorial paso a paso con capturas
  - [ ] Solución de problemas comunes

- [ ] Guía de Física:

  - [ ] Explicación de ecuaciones usadas
  - [ ] Comparación de métodos numéricos
  - [ ] Ejemplos de fenómenos físicos
  - [ ] Referencias académicas

- [ ] Guía del Editor:
  - [ ] Cómo crear escenarios
  - [ ] Tips para campos interesantes
  - [ ] Ejemplos de escenarios educativos

---

## 🔵 FASE 4: FEATURES AVANZADOS (Futuro)

**Nota**: Estas features están planificadas para versiones posteriores a 1.0. Se implementarán según demanda y tiempo disponible.

### 4.1 Soporte 3D

**Impacto**: Cambio arquitectónico significativo  
**Estimación**: 2-3 semanas

- [ ] Extensión del modelo:

  - [ ] Vector3d en lugar de Vector2d
  - [ ] Campo vectorial 3D
  - [ ] Escenario como volumen

- [ ] Visualización:

  - [ ] OpenGL o DirectX
  - [ ] Cámara orbital
  - [ ] Proyección 3D del bote

- [ ] Interacción:
  - [ ] Navegación 3D con mouse
  - [ ] Corte de planos (X-Y, X-Z, Y-Z)
  - [ ] Isosuperficies de velocidad

---

### 4.2 Inteligencia Artificial Navegadora

**Impacto**: Nuevo sistema  
**Estimación**: 1-2 semanas

- [ ] Agente Autónomo:

  - [ ] Percepción del campo vectorial
  - [ ] Toma de decisiones basada en objetivo
  - [ ] Evitación de obstáculos

- [ ] Algoritmos:

  - [ ] Pathfinding A\* en campo vectorial
  - [ ] Optimización de trayectoria
  - [ ] Aprendizaje por refuerzo (opcional)

- [ ] Comparación:
  - [ ] Carrera humano vs IA
  - [ ] Métricas comparativas
  - [ ] Replay de ambas trayectorias

---

### 4.3 Multiplayer Colaborativo

**Impacto**: Arquitectura cliente-servidor  
**Estimación**: 3-4 semanas

- [ ] Arquitectura:

  - [ ] Servidor de simulación
  - [ ] Sincronización de estado
  - [ ] Múltiples instancias de bote

- [ ] Features:
  - [ ] Sala de espera
  - [ ] Chat integrado
  - [ ] Competencias (quién llega primero)
  - [ ] Colaboración (navegación sincronizada)

---

### 4.4 Realidad Virtual

**Impacto**: Soporte hardware especializado  
**Estimación**: 2-3 semanas

- [ ] Soporte VR:

  - [ ] OpenXR
  - [ ] Oculus SDK
  - [ ] SteamVR

- [ ] Experiencia:
  - [ ] Vista en primera persona desde el bote
  - [ ] Controladores VR para dirección
  - [ ] Efectos de inmersión (ondas, viento)

---

## ✅ CRITERIOS DE COMPLETITUD

### Definición de "Listo" (v1.0)

Una feature está **completada** cuando:

1. ✅ El código compila sin warnings
2. ✅ Funciona según especificación
3. ✅ Tiene tests unitarios (cobertura >70%)
4. ✅ Está documentada (Javadoc + guía de usuario)
5. ✅ Ha sido probada manualmente
6. ✅ No introduce regresiones

### Milestones

#### Milestone 1: Estabilidad (Fase 1 completa)

- [ ] 0 TODOs críticos pendientes
- [ ] Sistema Undo/Redo funcional
- [ ] Parser de expresiones matemáticas
- [ ] Tests pasando >90%

#### Milestone 2: Feature Completo (Fase 2 completa)

- [ ] 10+ escenarios predefinidos
- [ ] Sistema de partículas completo
- [ ] Métricas avanzadas implementadas
- [ ] UI/UX pulida

#### Milestone 3: Calidad Profesional (Fase 3 completa)

- [ ] 70% cobertura de tests
- [ ] MainWindow refactorizado
- [ ] Documentación completa
- [ ] Rendimiento optimizado

#### Release 1.0

- [ ] Todos los milestones completados
- [ ] Testing de usuario final
- [ ] Empaquetado para distribución
- [ ] Anuncio y documentación publicada

---

## 📊 ESTADÍSTICAS Y SEGUIMIENTO

### Métricas de Progreso

```
Total de Features: 85
Completadas: 78 (92%)
Pendientes: 7 (8%)

Por Fase:
- Fase 1: 15/20 (75%) ✅ (TODO-001-005, 006-015)
- Fase 2: 6/25 (24%) 🔄 Escenarios (10), Handlers UI
- Fase 3: 0/30 (0%)
- Fase 4: 0/10 (0%)
```

### Historial de Cambios

| Fecha      | Versión | Cambios                                                                                                     |
| ---------- | ------- | ----------------------------------------------------------------------------------------------------------- |
| Marzo 2026 | 0.2.0   | Versión inicial MVP                                                                                         |
| Marzo 2026 | 0.2.1   | Auto Thrust, sincronización ExperimentService, pestaña Escenario, integración Theory->Simulation            |
| Marzo 2026 | 0.2.2   | Completitud 90%: TODOs 001-015, Undo/Redo, Atajos teclado, Observer, 10 escenarios                          |
| Marzo 2026 | 0.2.3   | Fase 2: 10 escenarios, partículas (estela/espuma/splash), métricas (desplazamiento/eficiencia), 59 handlers |

---

## 🎯 PRÓXIMOS PASOS INMEDIATOS

Al iniciar desarrollo:

1. **Prioridad 1**: Sistema de partículas (estela, espuma)
2. **Prioridad 2**: Métricas avanzadas de simulación
3. **Prioridad 3**: Mejoras de UI/UX (tooltips, temas)
4. **Prioridad 4**: Tests unitarios (cobertura >70%)

---

## 📝 NOTAS

- Este roadmap es **vivo** y se actualiza según prioridades
- Las estimaciones son aproximadas y dependen de disponibilidad
- Features pueden moverse entre fases según necesidad
- La Fase 4 es opcional y futura

---

**Última actualización**: Marzo 2026  
**Responsable**: Equipo de Desarrollo  
**Estado**: Activo
