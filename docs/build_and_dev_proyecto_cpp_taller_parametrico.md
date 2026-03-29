# BUILD_AND_DEV — Proyecto de C++: Taller Paramétrico de Navegación y Campos 2D

## 1. Propósito del documento

Este documento define la infraestructura de desarrollo del proyecto.

Su objetivo es dejar claro:

- cómo se compila el proyecto
- qué herramientas de build se usan
- cómo se manejan dependencias
- cómo se ejecutan pruebas
- cómo se formatea el código
- qué estructura mínima debe tener el entorno de desarrollo
- cómo arrancar el proyecto sin fricción innecesaria

Este documento no define la matemática ni la arquitectura funcional. Define la base técnica y operativa para poder trabajar de forma profesional, reproducible y mantenible.

---

## 2. Filosofía de la infraestructura

La infraestructura del proyecto debe ser:

- moderna pero no extravagante
- reproducible
- multiplataforma
- fácil de entender
- alineada con prácticas razonables de C++ contemporáneo
- suficientemente seria para un proyecto de portafolio

### Regla general
No se busca usar la herramienta más sofisticada posible, sino una combinación estable y reconocible.

---

## 3. Stack de build y desarrollo elegido

## 3.1 Herramientas principales

La base técnica recomendada del proyecto es:

- **CMake** como sistema de build
- **CMakePresets** como forma compartida de configuración
- **CTest** para ejecución de pruebas
- **vcpkg** para dependencias externas
- **clang-format** para formato de código
- **clang-tidy** como herramienta opcional de análisis estático

## 3.2 Justificación general

### CMake
Se elige por ser el estándar práctico más común para proyectos modernos de C++.

### CMakePresets
Se elige para evitar instrucciones caóticas y permitir configuraciones compartidas del proyecto.

### CTest
Se elige por integrarse naturalmente con CMake.

### vcpkg
Se elige como gestor de dependencias por su enfoque pragmático, multiplataforma y buena integración con CMake.

### clang-format
Se elige para mantener formato consistente desde el principio.

### clang-tidy
Se deja como opcional pero muy recomendable para detectar problemas tempranos.

---

## 4. Requisitos mínimos del entorno

## 4.1 Compiladores deseados
El proyecto debe compilarse al menos con una combinación moderna y razonable.

### Compiladores objetivo
- GCC moderno
- Clang moderno
- MSVC moderno

No es necesario perseguir compatibilidad con compiladores muy antiguos.

## 4.2 Estándar de C++
### Recomendación
Usar **C++20** como base del proyecto.

### Razones
- suficientemente moderno
- ampliamente soportado
- cómodo para modelado, utilidades y buena expresividad
- evita complicaciones más recientes que todavía podrían ser innecesarias para este proyecto

## 4.3 Generador de build recomendado
### Recomendación principal
**Ninja** cuando sea posible.

### Alternativas
- Visual Studio generator en Windows
- Unix Makefiles si hace falta

### Criterio
Ninja debe ser el camino preferido por simplicidad y velocidad cuando esté disponible.

---

## 5. Política de versiones de herramientas

El proyecto debe documentar versiones mínimas recomendadas.

## 5.1 CMake
Se recomienda fijar una versión mínima suficientemente moderna para que el proyecto pueda usar presets y prácticas actuales sin peleas innecesarias.

## 5.2 Compiladores
Debe fijarse una línea base razonable, pero sin obsesionarse con soportar entornos obsoletos.

## 5.3 Dependencias
Las versiones deben declararse de forma reproducible, idealmente con manifiesto o lock equivalente según el flujo usado.

---

## 6. Organización de build

## 6.1 Build out-of-source
El proyecto debe compilarse **fuera del árbol fuente**.

### Regla escrita en piedra
No se compila directamente dentro de `src/` ni se llena el repositorio de artefactos temporales.

## 6.2 Carpetas sugeridas

```text
/build/
/out/
```

O usar exclusivamente carpetas generadas por presets.

## 6.3 Artefactos a ignorar
Deben ignorarse en Git:
- binarios
- objetos
- cachés de build
- carpetas del generador
- salidas de IDE

---

## 7. Estructura mínima del proyecto para build

```text
proyecto_cpp/
  CMakeLists.txt
  CMakePresets.json
  vcpkg.json
  .clang-format
  .gitignore
  README.md
  docs/
  assets/
  data/
  src/
  tests/
```

### Archivos clave
- `CMakeLists.txt`: punto de entrada del build
- `CMakePresets.json`: presets compartidos
- `vcpkg.json`: manifiesto de dependencias
- `.clang-format`: reglas de formato

---

## 8. Política de CMake

## 8.1 Estilo recomendado
El proyecto debe usar **target-based CMake**.

### Eso significa
- evitar configuración global innecesaria
- usar `target_link_libraries()`
- usar `target_include_directories()`
- usar `target_compile_definitions()`
- usar `target_compile_features()`

### Regla
Las propiedades se aplican a targets concretos, no al mundo entero si se puede evitar.

## 8.2 Estructura modular en CMake
Se recomienda organizar targets por módulos o bibliotecas internas.

### Posibles targets internos
- `project_domain`
- `project_simulation`
- `project_education`
- `project_rendering`
- `project_application`
- `project_presentation`
- `project_main`

Esto ayuda a:
- separar compilación
- aislar responsabilidades
- facilitar pruebas
- mantener dependencia dirigida

## 8.3 Política de `CMakeLists.txt`

### Recomendación
- un `CMakeLists.txt` raíz claro y corto
- `CMakeLists.txt` por subdirectorio donde sea útil
- evitar magia excesiva
- preferir claridad sobre metaprogramación de CMake

---

## 9. Presets de CMake

## 9.1 Propósito
Los presets deben servir para compartir configuraciones comunes del proyecto.

## 9.2 Presets sugeridos

### `dev-debug`
Para desarrollo diario.

### `dev-release`
Para probar comportamiento en release.

### `ci-debug`
Para integración continua o validación automatizada.

### `tests-debug`
Para construir y ejecutar pruebas de forma explícita.

## 9.3 Ideas de configuración
Cada preset puede fijar:
- generador
- tipo de build
- toolchain file si aplica
- activación de pruebas
- flags de desarrollo
- flags de sanitizers si se añaden

---

## 10. Dependencias externas

## 10.1 Estrategia recomendada
El proyecto debe manejar dependencias externas con **vcpkg** en modo manifiesto.

## 10.2 Razón
Esto permite:
- documentar dependencias de manera explícita
- facilitar reproducibilidad
- integrar bibliotecas con CMake de forma más ordenada

## 10.3 Dependencias previstas
Estas pueden variar, pero conceptualmente el proyecto podría necesitar:

- `wxwidgets`
- biblioteca de expresiones matemáticas futura si se adopta
- alguna biblioteca de gráficos o utilidades si realmente se necesita

### Regla importante
No añadir dependencias por entusiasmo. Cada biblioteca externa debe justificar su existencia.

## 10.4 Archivo de manifiesto
Debe existir un archivo como:

```text
vcpkg.json
```

con las dependencias declaradas de forma explícita.

---

## 11. Política de pruebas

## 11.1 Enfoque general
Las pruebas deben enfocarse primero en:
- dominio
- simulación
- lógica de aplicación

La interfaz gráfica no debe ser el primer frente de pruebas automáticas.

## 11.2 Herramienta base
**CTest** debe ser el punto de integración para ejecutar pruebas desde el build.

## 11.3 Niveles de prueba recomendados

### Pruebas de dominio
- escenario
- campo
- bote
- fuente de energía
- consumo energético

### Pruebas de simulación
- integradores
- evolución temporal
- potencia y energía
- eventos básicos

### Pruebas de aplicación
- creación de escenario
- ejecución de caso de uso
- guardado/carga

### Pruebas de integración
- flujo básico de experimento
- lectura de teoría asociada
- persistencia simple

## 11.4 Regla de prioridad
Primero se prueban las partes que no dependen de GUI.

---

## 12. Política de formato de código

## 12.1 Herramienta elegida
**clang-format**

## 12.2 Archivo de configuración
Debe existir:

```text
.clang-format
```

## 12.3 Objetivo
- mantener estilo consistente
- evitar discusiones inútiles sobre espacios y llaves
- facilitar revisiones

## 12.4 Criterio práctico
Elegir un estilo base conocido y ajustarlo solo si hace falta.

No se debe crear un estilo exótico por capricho.

---

## 13. Política de análisis estático

## 13.1 Herramienta opcional recomendada
**clang-tidy**

## 13.2 Uso sugerido
- ejecutarlo de forma manual al inicio
- integrarlo en etapas posteriores
- enfocarlo en errores y malas prácticas relevantes

## 13.3 Regla
No debe bloquear el inicio del proyecto si todavía no hay tiempo para afinarlo, pero sí conviene incorporarlo pronto.

---

## 14. Política de warnings

El proyecto debe compilar con warnings razonables activados.

### Recomendación
Tener una política seria de advertencias, al menos en desarrollo.

### Regla práctica
- corregir warnings relevantes
- evitar ignorarlos sistemáticamente
- no activar combinaciones imposibles de mantener en todas las plataformas desde el día uno

---

## 15. Política de sanitizers

## 15.1 Recomendación
Agregar soporte opcional para sanitizers en builds de desarrollo cuando el compilador lo permita.

### Sanitizers útiles
- AddressSanitizer
- UndefinedBehaviorSanitizer

## 15.2 Criterio
No son obligatorios en el primer minuto, pero sí son muy recomendables cuando empiece a crecer el proyecto.

---

## 16. Política de assets y datos

## 16.1 Separación
Los assets y datos no deben mezclarse con el código fuente.

### Carpetas sugeridas
- `assets/`
- `data/`

## 16.2 Ejemplos
En `data/` pueden vivir:
- escenarios de ejemplo
- fichas de teoría
- catálogos de combustibles

En `assets/` pueden vivir:
- iconos
- texturas simples
- imágenes auxiliares

---

## 17. Política de configuración

## 17.1 Configuración del proyecto
La configuración de build debe vivir en:
- `CMakePresets.json`
- `vcpkg.json`
- archivos del entorno de desarrollo cuando aplique

## 17.2 Configuración del usuario
Idealmente, configuraciones del usuario no deben contaminar la configuración compartida del proyecto.

---

## 18. Flujo de trabajo recomendado

## 18.1 Flujo mínimo para un desarrollador
1. clonar repositorio
2. instalar herramientas necesarias
3. resolver dependencias con vcpkg
4. configurar con preset
5. compilar
6. ejecutar pruebas
7. correr la aplicación

## 18.2 Flujo de cambio típico
1. cambiar código
2. compilar con preset de desarrollo
3. ejecutar pruebas
4. formatear código
5. revisar warnings
6. confirmar cambios

---

## 19. Recomendación de inicio práctico

Para empezar sin dolor, la primera etapa del proyecto debería incluir:

- `CMakeLists.txt` raíz
- `CMakePresets.json`
- `vcpkg.json`
- target principal ejecutable
- uno o dos targets internos mínimos
- `tests/` con al menos una prueba simple
- `.clang-format`

No hace falta montar desde el día uno una infraestructura gigante.

---

## 20. Integración futura con CI

No es obligatorio al principio, pero la infraestructura debe dejar el camino preparado para integración continua.

### Lo que conviene dejar listo desde temprano
- presets estables
- tests ejecutables por CTest
- formato de código consistente
- dependencias declaradas

---

## 21. Recomendaciones específicas para este proyecto

## 21.1 Targets internos sugeridos al principio

### `core_domain`
Entidades y reglas puras.

### `core_simulation`
Integradores y evolución temporal.

### `app_gui`
Ventanas, paneles y canvas.

### `app_main`
Ejecutable final.

Con eso basta para arrancar.

## 21.2 Estrategia de compilación
Empezar con pocos módulos y luego dividir si realmente hace falta.

## 21.3 Dependencias
Mantener pocas dependencias externas en la primera etapa.

---

## 22. Riesgos a evitar

- meter demasiadas herramientas desde el inicio
- depender de instrucciones manuales no documentadas
- compilar dentro del árbol fuente
- mezclar configuración personal con compartida
- crear una estructura de CMake demasiado sofisticada para un proyecto aún pequeño
- agregar dependencias innecesarias por entusiasmo
- intentar resolver todas las plataformas y todos los IDEs con exactitud quirúrgica desde el día uno

---

## 23. Decisiones escritas en piedra

1. el proyecto usará **CMake** como sistema de build
2. el proyecto usará **CMakePresets** para configuraciones compartidas
3. el proyecto usará **CTest** como base de pruebas integradas al build
4. el proyecto usará **vcpkg** para gestionar dependencias externas
5. el proyecto adoptará **build out-of-source**
6. el estándar base recomendado será **C++20**
7. el proyecto usará **clang-format** para mantener estilo consistente
8. `clang-tidy` será recomendable, aunque no obligatorio en la primera etapa
9. la infraestructura inicial debe ser pequeña, clara y ampliable
10. se priorizará una experiencia de desarrollo reproducible sobre la sofisticación innecesaria

---

## 24. Frase oficial del documento

**La infraestructura de desarrollo del proyecto se basará en CMake, CMakePresets, CTest y vcpkg para ofrecer un flujo de build moderno, reproducible y multiplataforma, complementado con formateo consistente y una organización mínima pero sólida que permita iniciar el proyecto sin sobreingeniería.**

