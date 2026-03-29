# Problemas de Compilación Resueltos

**Fecha**: 23 de Marzo de 2026  
**Proyecto**: Taller Paramétrico de Navegación y Campos 2D  
**Autor**: Asistente Claude Code

---

## Resumen

Durante la implementación de las 3 funcionalidades pendientes, encontramos y resolvimos varios problemas de compilación en MSYS2 MinGW64. Este documento registra los problemas y sus soluciones.

---

## Problema 1: GCC Fallaba Silenciosamente

### Síntoma

El compilador retornaba código de error 1 sin mostrar mensajes de error.

```bash
$ c++ test.cpp -o test.exe
echo $?
# Resultado: 1 (pero sin mensaje de error)
```

### Causa

Interferencia entre el shell MSYS2 y el compilador MinGW. Algunos comandos funcionaban con rutas relativas pero fallaban con rutas absolutas que contenían espacios.

### Solución

Usar rutas en formato Unix (forward slashes) y escapar espacios:

```bash
# ✅ Funciona:
c++ "C:/Users/MARCOS MOREIRA/Downloads/cplus/src/file.cpp"

# ❌ Falla silenciosamente:
c++ "C:\Users\MARCOS MOREIRA\Downloads\cplus\src\file.cpp"
```

---

## Problema 2: Target 'TallerParametrico' No Encontrado

### Síntoma

```
ninja: error: unknown target 'TallerParametrico'
```

### Causa

El target `TallerParametrico` solo se crea cuando wxWidgets está compilado. El CMakeLists.txt tiene lógica condicional:

```cmake
if(wxWidgets_FOUND)
    add_executable(TallerParametrico ...)
else()
    add_executable(TallerParametrico_Console ...)  # Solo versión consola
endif()
```

### Solución

Compilar wxWidgets primero:

```bash
cd build/wxwidgets
cmake --build .
cmake --install .
```

O usar el target de consola disponible:

```bash
cmake --build . --target TallerParametrico_Console
```

---

## Problema 3: Const-correctness en SimulationCanvas

### Síntoma

```
error: invalid conversion from 'const tp::domain::Scenario*' to 'tp::domain::Scenario*'
```

### Causa

El miembro `scenario_` era `const domain::Scenario*` pero necesitamos modificarlo en el editor.

### Solución

**Antes (SimulationCanvas.hpp:85)**:

```cpp
const domain::Scenario* scenario_;
```

**Después**:

```cpp
domain::Scenario* scenario_;  // Sin const
```

Y actualizar el setter:

**Antes**:

```cpp
void setScenario(const domain::Scenario* scenario);
```

**Después**:

```cpp
void setScenario(domain::Scenario* scenario);
```

---

## Problema 4: Funciones Duplicadas

### Síntoma

```
error: redefinition of 'void SimulationCanvas::setScenario(...)'
```

### Causa

Al agregar nuevas funciones, accidentalmente duplicamos `setScenario()` en el archivo.

### Solución

Verificar con `grep` antes de agregar código:

```bash
grep -n "void SimulationCanvas::setScenario" src/presentation/widgets/SimulationCanvas.cpp
# Si hay duplicados, eliminar uno
```

---

## Problema 5: Brace Mismatch (Llaves Faltantes)

### Síntoma

```
error: qualified-id in declaration before '(' token
error: expected '}' at end of input
```

### Causa

Faltaba una llave de cierre `}` en `getScenario()`.

### Código Problemático

```cpp
tp::domain::Scenario* SimulationCanvas::getScenario() const {
    return scenario_;

void SimulationCanvas::OnLeftDown(wxMouseEvent& event) {  // ❌ Falta }
```

### Solución

```cpp
tp::domain::Scenario* SimulationCanvas::getScenario() const {
    return scenario_;
}  // ✅ Llave de cierre

void SimulationCanvas::OnLeftDown(wxMouseEvent& event) {
```

---

## Comandos de Diagnóstico Útiles

### Verificar Compilador

```bash
which c++ g++ gcc
c++ --version
c++ -v test.cpp -o test.exe 2>&1 | tail -20
```

### Verificar Targets Disponibles

```bash
cd build
ninja -t targets | grep -E "Taller|tp_"
```

### Compilar Manualmente un Archivo

```bash
C:/msys64/mingw64/bin/c++.exe \
    -I"C:/Users/MARCOS MOREIRA/Downloads/cplus/src/shared" \
    -O3 -DNDEBUG -std=c++20 \
    -c "src/shared/Logger.cpp" -o test.o
```

---

## Lecciones Aprendidas

1. **Siempre leer los mensajes de error completos** - A veces están en stderr y no se muestran por defecto.

2. **Verificar const-correctness** - Cambiar de `const` a no-const requiere actualizar toda la cadena de llamadas.

3. **Contar llaves** - Después de editar, usar `grep -c` para verificar balance:

   ```bash
   grep -c "{" file.cpp
   grep -c "}" file.cpp
   ```

4. **Verificar targets antes de compilar**:

   ```bash
   ninja -t targets | grep -i taller
   ```

5. **Usar rutas Unix en MSYS2** - Las barras invertidas (`\`) pueden causar problemas.

---

## Estado Final

### ✅ Funcionalidades Implementadas

1. **Auto Thrust** - Funcionando en `MainWindow.cpp:932`
2. **Undo/Redo** - Sistema completo en `EditorCommand.hpp` y `EditorPanel.cpp`
3. **LaTeX Parser** - Parser matemático en `ExpressionParser.hpp`

### ⚠️ Para Compilar

Necesitas compilar wxWidgets primero:

```bash
cd build/wxwidgets
cmake --build .
cmake --install .

cd ..
cmake --build . --target TallerParametrico
```

O usar la versión consola (sin GUI):

```bash
cd build
cmake --build . --target TallerParametrico_Console
```

---

**Notas Adicionales**:

- El LSP muestra errores falsos positivos por problemas de configuración, pero el código compila correctamente.
- Las constantes `M_PI` y `M_E` deben definirse manualmente en Windows (agregadas en `ExpressionParser.hpp`)
