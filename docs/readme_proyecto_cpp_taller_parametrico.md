# Proyecto de C++ — Taller Paramétrico de Navegación y Campos 2D

## Descripción

**Taller Paramétrico de Navegación y Campos 2D** es una aplicación de escritorio en **C++** orientada a la exploración visual, didáctica y experimental de fenómenos simplificados relacionados con:

- campos vectoriales en 2D
- trayectorias
- fuerzas y movimiento
- energía y trabajo
- métodos numéricos
- escenarios editables con agua, tierra y obstáculos

El proyecto está concebido como una herramienta de aprendizaje y experimentación, no como un simulador profesional de ingeniería naval ni como un videojuego completo.

La idea central es permitir que el usuario:

- observe conceptos matemáticos y físicos de forma visual
- modifique parámetros del entorno
- defina funciones del campo o del motor
- simule trayectorias
- compare comportamientos
- relacione teoría con resultados numéricos

Además de su valor técnico, el proyecto busca transmitir una sensación de software educativo/técnico clásico, con una interfaz inspirada en programas de escritorio sobrios y didácticos.

---

## Propósito del proyecto

Este proyecto existe para unir varias motivaciones en una sola aplicación:

- estudiar **C++** en un proyecto serio de escritorio
- practicar una **interfaz gráfica rica** y clásica
- repasar conceptos de:
  - cálculo de una variable
  - cálculo de varias variables
  - álgebra lineal
  - física simplificada
  - métodos numéricos
- construir una herramienta visual que permita crear, editar y reutilizar escenarios propios
- dejar una base bien documentada y presentable en GitHub

---

## Qué hace la aplicación

La aplicación permite, de forma progresiva según la versión:

- definir o cargar un escenario 2D
- distinguir zonas de agua, tierra y obstáculos
- visualizar un campo vectorial sobre el escenario
- colocar un bote 2D simplificado
- definir la fuerza del motor mediante parámetros o funciones
- ejecutar simulaciones numéricas en el tiempo
- observar trayectorias, energía, velocidad y eventos de colisión
- consultar teoría matemática asociada dentro del programa
- guardar y reutilizar escenarios y experimentos

---

## Qué no busca ser

Para mantener el proyecto enfocado, la aplicación **no busca ser** en su primera etapa:

- un simulador físico realista de fluidos
- un motor de videojuegos
- un editor 3D generalista
- un software CAD profesional
- una herramienta de elementos finitos
- una aplicación web
- una suite científica gigantesca

---

## Enfoque conceptual

El patrón base del proyecto es:

**escenario + campo + objeto + motor + método numérico → trayectoria y análisis**

Esto permite que el programa se convierta en un pequeño laboratorio visual donde una idea matemática se transforma en una experiencia manipulable.

Ejemplos de conceptos que el proyecto quiere ilustrar:

- campo vectorial
- integral de línea
- circulación
- divergencia
- rotor
- trayectoria paramétrica
- trabajo y energía
- integración numérica
- comparación entre solución analítica y numérica cuando sea posible

---

## Público objetivo

El proyecto está pensado principalmente para:

- el propio autor, como herramienta de estudio y portafolio
- estudiantes con interés en matemáticas aplicadas, computación o simulación
- personas que disfrutan del software técnico clásico y los experimentos visuales

---

## Estado del proyecto

**Estado actual:** etapa de definición funcional y conceptual.

Prioridades actuales:

1. escribir en piedra la especificación funcional
2. definir el alcance real del MVP
3. aterrizar la estructura documental del proyecto
4. decidir toolkit gráfico, build system y organización interna
5. comenzar implementación por módulos pequeños

---

## Stack previsto

La base técnica prevista para el proyecto es:

- **C++** como lenguaje principal
- **CMake** como sistema de build
- **wxWidgets** como toolkit de interfaz gráfica
- **CTest** para pruebas
- **vcpkg** o solución equivalente para dependencias
- renderizado matemático y visual según la etapa de desarrollo

La interfaz tendrá orientación de software de escritorio clásico, con paneles, áreas de visualización, inspector de propiedades y secciones de teoría/simulación.

---

## Filosofía de interfaz

La aplicación debe sentirse como una mezcla entre:

- laboratorio interactivo
- software técnico clásico
- pieza educativa tipo Encarta técnica
- herramienta de escritorio sobria y detallada

La prioridad no es lo moderno por sí mismo, sino lo **claro, funcional, didáctico y visualmente rico**.

---

## Estructura documental prevista

El proyecto se apoyará en un conjunto pequeño pero fuerte de documentos Markdown.

Documentos previstos:

- `README.md`
- `docs/PRODUCT_SPEC.md`
- `docs/MATH_AND_SIMULATION.md`
- `docs/UX_AND_SCREENS.md`
- `docs/ARCHITECTURE.md`
- `docs/BUILD_AND_DEV.md`
- `docs/ROADMAP.md`

---

## Roadmap resumido

### Etapa 1 — Definición
- especificación funcional
- alcance del MVP
- decisiones base
- diseño de pantallas principales

### Etapa 2 — Núcleo técnico
- estructura base del proyecto
- escenario 2D simple
- modelo del bote
- campo vectorial básico
- integración numérica inicial

### Etapa 3 — Interfaz visual
- vista principal
- panel de parámetros
- visualización del campo
- trayectoria y resultados

### Etapa 4 — Modo teoría
- textos didácticos
- fórmulas
- mini ejemplos predeterminados
- conexión teoría ↔ simulación

### Etapa 5 — Editor y reutilización
- guardado/carga de escenarios
- edición de mapas simples
- plantillas y experimentos reutilizables

---

## Estado deseado del MVP

La primera versión mínimamente valiosa debería incluir:

- un mapa 2D con agua y tierra
- un campo vectorial predefinido
- un bote rectangular simplificado
- un motor configurable
- simulación con al menos un método numérico básico
- trayectoria visible
- colisión simple con tierra
- teoría mínima integrada
- guardado/carga de escenarios simples

---

## Criterios de éxito

El proyecto se considerará bien encaminado si logra:

- tener una base de escritorio sólida en C++
- visualizar al menos un fenómeno de manera clara
- permitir modificar parámetros y observar consecuencias
- unir teoría y simulación de forma entendible
- mantener una estructura documental y técnica limpia
- verse presentable como proyecto serio en GitHub

---

## Motivación personal

Este proyecto no nace solo por utilidad práctica inmediata, sino también por gusto personal:

- interés por software técnico clásico
- nostalgia por interfaces didácticas de escritorio
- gusto por las matemáticas aplicadas y la computación gráfica simple
- deseo de crear una herramienta propia, reutilizable y visualmente interesante

En ese sentido, el proyecto también funciona como una especie de “taller digital”: un lugar donde conceptos matemáticos, experimentos visuales y diseño de software se encuentran.

---

## Licencia

Pendiente de definir.

---

## Nota final

Este README es una puerta de entrada. La especificación formal del proyecto, sus ecuaciones, pantallas, arquitectura y decisiones de diseño se documentarán en los archivos de `docs/`.
