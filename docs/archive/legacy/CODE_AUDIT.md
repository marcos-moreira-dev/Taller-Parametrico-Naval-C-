# Auditoría de Código - Taller Paramétrico

**Fecha**: Marzo 2026  
**Auditor**: Asistente Claude Code  
**Versión**: 1.0

---

## 📊 Resumen Ejecutivo

| Métrica                     | Valor   |
| --------------------------- | ------- |
| Archivos fuente analizados  | 40+     |
| Errores críticos corregidos | 5       |
| TODOs pendientes            | 16      |
| Deuda técnica alta          | 3 items |
| Code smells                 | 8       |
| Documentación faltante      | Media   |

---

## 🔴 Deuda Técnica ALTA (Corregir ASAP)

### 1. **MainWindow.cpp - God Class**

**Archivo**: `src/presentation/main_window/MainWindow.cpp`  
**Líneas**: 2,052  
**Severidad**: 🔴🔴🔴 Alta

**Problema**: Clase monolítica que viola el Principio de Responsabilidad Única (SRP).
Contiene:

- Lógica de UI (500+ líneas)
- Manejo de eventos (600+ líneas)
- Lógica de simulación (400+ líneas)
- Configuración de widgets (300+ líneas)
- Navegación entre paneles (200+ líneas)

**Impacto**:

- Difícil de mantener
- Difícil de testear
- Alto acoplamiento

**Solución Recomendada**:

```cpp
// Dividir en controladores especializados:
class SimulationController;    // Lógica de simulación
class UINavigationController; // Navegación entre paneles
class ConfigurationManager;    // Gestión de configuración
class EventDispatcher;         // Manejo centralizado de eventos
```

---

### 2. **TheoryPanel.cpp - Acoplamiento Fuerte**

**Archivo**: `src/presentation/panels/TheoryPanel.cpp`  
**Líneas**: 551  
**Severidad**: 🔴🔴 Media-Alta

**Problema**: Depende directamente de MainWindow (línea 529):

```cpp
// TODO: Notificar al MainWindow para cambiar a modo simulación
```

**Impacto**: Circular dependencies, difícil de testear.

**Solución**: Usar patrón Observer o Mediator.

---

### 3. **SimulationCanvas.hpp - Punteros Crudos**

**Archivo**: `src/presentation/widgets/SimulationCanvas.hpp`  
**Severidad**: 🔴🔴 Media

**Problema**: Usa punteros crudos en lugar de smart pointers:

```cpp
domain::Scenario* scenario_;  // Debería ser std::unique_ptr o std::shared_ptr
```

**Riesgo**: Memory leaks, dangling pointers.

**Solución**: Migrar a `std::unique_ptr<domain::Scenario>`.

---

## 🟡 Deuda Técnica MEDIA

### 4. **Código Duplicado - Manejo de Errores**

**Ubicación**: Múltiples archivos

**Problema**: Patrón repetido:

```cpp
if (error) {
    LOG_ERROR_MACRO("Error en X");
    return false;
}
```

**Solución**: Crear utilidad `Result<T>` o usar excepciones consistentemente.

---

### 5. **Magic Numbers**

**Ubicación**: Múltiples archivos

**Ejemplos**:

- `33` (milisegundos) - debería ser constante `DEFAULT_TIMESTEP_MS`
- `0.001` (epsilon) - debería ser `EPSILON` o `SMALL_VALUE`
- `1000`, `100`, `10` - constantes de conversión

**Solución**: Centralizar en archivo de constantes.

---

### 6. **Inclusión de Headers**

**Problema**: `using namespace` en headers (VectorField.hpp línea 27)

**Impacto**: Polución de namespace global.

**Solución**:

```cpp
// En lugar de:
using namespace tp::shared;

// Usar:
using Vec2d = tp::shared::Vec2d;  // Solo lo necesario
```

---

## 🔵 Deuda Técnica BAJA

### 7. **Nombres de Variables Inconsistentes**

- `formulaText_` vs `formulaWidget_` (YA CORREGIDO)
- `dt` vs `timeStep` vs `deltaTime`

### 8. **Comentarios de Código Muerto**

**Archivos**: MainWindow.cpp, SimulationCoordinator.cpp

**Ejemplo**:

```cpp
// Código comentado grande
/*
void funcionAntigua() {
    ...
}
*/
```

**Solución**: Eliminar o mover a documentación.

---

## 📝 TODOs Pendientes (16 encontrados)

### Prioridad ALTA:

1. `MainWindow.cpp:714` - Obtener valor real del slider
2. `MainWindow.cpp:760` - Obtener selección real
3. `MainWindow.cpp:769` - Obtener dt del slider

### Prioridad MEDIA:

4. `MainWindow.cpp:1207` - Actualizar lista desde ExperimentService
5. `MainWindow.cpp:1641` - Sincronizar con ExperimentService
6. `MainWindow.cpp:1667` - Crear pestaña de escenario

### Prioridad BAJA:

7. `TheoryPanel.cpp:510` - Mantener selección al filtrar
8. `TheoryPanel.cpp:529` - Notificar a MainWindow (cambiar modo)
9. `EditorPanel.cpp:255` - Actualizar visual de botones
10. `EditorPanel.cpp:286` - Actualizar tamaño del cursor
11. `EditorPanel.cpp:291` - Activar/desactivar snap
12. `EditorPanel.cpp:297` - Crear nuevo escenario vacío
13. `EditorPanel.cpp:306` - Redimensionar escenario
14. `EditorPanel.cpp:369` - Actualizar tipo de campo
15. `EditorPanel.cpp:374` - Actualizar parámetros del campo
16. `EditorPanel.cpp:379` - Actualizar parámetros desde SpinCtrl

---

## 🎯 Code Smells Identificados

### 1. **Feature Envy**

`TheoryPanel` conoce demasiado sobre `MainWindow`.

### 2. **Primitive Obsession**

Uso de `double` para ángulos sin tipo `Radians` o `Degrees`.

### 3. **Long Parameter List**

```cpp
VectorField::channelWithObstacle(double centerY, double width,
                                  double obstacleX, double flowSpeed);
// 4 parámetros - considerar struct ChannelConfig
```

### 4. **Switch Statements**

En `SimulationCoordinator::setIntegrationMethod()` - podría usar factory.

### 5. **Temporary Field**

Variables miembro que solo se usan en ciertos métodos.

### 6. **Message Chains**

```cpp
runner_->getResult().states.back().position.x;
// Demasiado acoplamiento
```

### 7. **Middle Man**

Algunos métodos solo delegan sin añadir valor.

### 8. **Inappropriate Intimacy**

Clases que conocen detalles internos de otras.

---

## 📚 Documentación Faltante

### Archivos sin Javadoc completo:

- [ ] `MainWindow.hpp/cpp` - Solo documentación básica
- [ ] `EditorPanel.cpp` - Métodos sin documentar
- [ ] `SimulationCanvas.cpp` - Sin comentarios de implementación

### Necesitan más ejemplos:

- [ ] `VectorField` - Solo tiene ejemplos básicos
- [ ] `PhysicsModel` - Sin ejemplos de uso

---

## ✅ Fortalezas del Código

1. **Arquitectura en capas** bien definida
2. **Separación de concerns** (Domain, Simulation, Presentation)
3. **Uso de C++ moderno** (C++20, lambdas, smart pointers donde aplica)
4. **Sistema de logging** consistente
5. **Manejo de errores** con LOG_ERROR_MACRO
6. **Factory methods** para campos vectoriales

---

## 🚀 Recomendaciones Prioritarias

### Corto plazo (1-2 semanas):

1. ✅ **Corregir errores de compilación** (YA HECHO)
2. 🔴 **Refactorizar MainWindow.cpp** - Dividir en controladores
3. 🟡 **Eliminar código muerto** de comentarios
4. 🟡 **Centralizar magic numbers**

### Mediano plazo (1 mes):

1. 🟡 **Implementar 16 TODOs** pendientes
2. 🟡 **Migrar punteros crudos** a smart pointers
3. 🔵 **Completar documentación** Javadoc
4. 🔵 **Agregar más tests** unitarios

### Largo plazo (2-3 meses):

1. 🔵 **Implementar patrón MVC** completo
2. 🔵 **Sistema de plugins** para campos vectoriales
3. 🔵 **Serialización** mejorada de escenarios

---

## 📈 Métricas de Calidad

### Cobertura estimada:

- **Documentación**: 60% (mejorable)
- **Tests**: 20% (baja)
- **Cohesión**: 70% (algunas clases grandes)
- **Acoplamiento**: 65% (puede mejorar)

### Complejidad ciclomática estimada:

- `MainWindow.cpp`: ⚠️ Alta (>50)
- `TheoryPanel.cpp`: ⚠️ Media-Alta (30-40)
- `VectorField.cpp`: ✅ Baja (<10)

---

## 🎓 Lecciones Aprendidas

1. **Siempre verificar includes** después de crear nuevos archivos
2. **Definir M_PI** explícitamente en Windows
3. **Usar `if(TARGET ...)`** en CMake para targets condicionales
4. **Copiar wxWidgets** a build/ antes de configurar CMake
5. **Mantener archivos <500 líneas** cuando sea posible

---

**Conclusión**: El código base es sólido pero necesita refactoring en la capa de presentación. La deuda técnica es manejable pero debería abordarse progresivamente.
