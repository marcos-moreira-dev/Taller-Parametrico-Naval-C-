# ARCHITECTURE — Proyecto de C++: Taller Paramétrico de Navegación y Campos 2D

## 1. Propósito del documento

Este documento define la arquitectura general del proyecto.

Su objetivo es dejar claro:

- qué estilo arquitectónico tendrá la aplicación
- qué módulos principales existirán
- cómo se separan responsabilidades
- cómo fluyen los datos entre interfaz, dominio, simulación y persistencia
- cómo convivirán los distintos subdominios del proyecto sin convertirse en caos
- cómo incorporar extensiones futuras sin rehacer toda la base

Este documento no reemplaza la especificación funcional ni el documento matemático. Su función es traducir esas ideas a una estructura de software sana.

---

## 2. Problema arquitectónico principal

Este proyecto no tiene un solo dominio puro. En realidad mezcla varias áreas:

- simulación matemática y numérica
- representación visual 2D
- posible estilización visual pseudo-3D o 3D ligera en el futuro
- edición de escenarios
- contenido educativo y teórico
- modelado del bote como objeto técnico simplificado
- modelado de propulsión y consumo energético
- guardado, carga y exportación

Si todo eso se mezcla sin orden, la aplicación se convierte rápidamente en una masa confusa.

Por tanto, la arquitectura debe priorizar:

- separación explícita de subdominios
- aislamiento del núcleo de simulación
- desacoplamiento entre GUI y cálculo
- crecimiento incremental
- una sola aplicación, pero con módulos internos bien definidos

---

## 3. Estilo arquitectónico general

## 3.1 Decisión principal
La aplicación se implementará como un **monolito modular de escritorio**.

### Qué significa aquí
- una sola aplicación
- un solo proceso principal
- una sola interfaz unificada
- una sola base documental
- varios módulos internos bien separados

### Por qué se elige esto
- encaja con software de escritorio clásico
- es suficiente para el alcance
- evita complejidad artificial
- sigue permitiendo una buena separación de responsabilidades

---

## 3.2 Arquitectura base por capas
La solución debe organizarse en capas amplias.

### Capas propuestas

#### 1. Capa de presentación
Responsable de:
- ventanas
- paneles
- menús
- toolbar
- canvas
- formularios
- interacción del usuario

#### 2. Capa de aplicación
Responsable de:
- casos de uso
- coordinación entre módulos
- orquestación de simulaciones
- carga y guardado
- flujos entre modos

#### 3. Capa de dominio
Responsable de:
- entidades principales
- reglas del sistema
- definiciones del escenario
- definición del bote
- definición del motor
- teoría estructurada
- validaciones conceptuales

#### 4. Capa de simulación
Responsable de:
- ecuaciones del modelo
- integración numérica
- evolución temporal
- resultados derivados
- comparación entre métodos

#### 5. Capa de infraestructura
Responsable de:
- persistencia
- archivos
- configuración
- recursos visuales
- exportación
- renderizado técnico específico

---

## 3.3 Enfoque de presentación recomendado
La capa de presentación debe inspirarse más en:

- **Presentation Model**
o
- **MVP ligero**

que en un MVC rígido clásico.

### Razón
La interfaz será rica en paneles, propiedades, estados, resultados y cambios contextuales. Un controlador único de estilo MVC puede crecer demasiado y volverse incómodo.

### Idea práctica
- las vistas dibujan y capturan eventos
- los presenters o view-models coordinan la lógica visual
- la capa de aplicación ejecuta casos de uso
- el dominio y la simulación no dependen de widgets

---

## 4. Subdominios principales del proyecto

Para que la arquitectura sea sana, el proyecto debe reconocer explícitamente sus subdominios.

## 4.1 Subdominio de escenario
Representa el mundo 2D editable.

Incluye:
- dimensiones
- celdas o regiones
- agua
- tierra
- obstáculos
- metadatos del escenario

## 4.2 Subdominio de campo
Representa el comportamiento del entorno sobre el espacio.

Incluye:
- campos predefinidos
- composición de campos
- evaluación en puntos
- parámetros del campo

## 4.3 Subdominio de navegación y bote
Representa el objeto móvil principal.

Incluye:
- geometría simplificada
- masa
- orientación
- parámetros físicos
- estado dinámico

## 4.4 Subdominio de propulsión y energía
Este subdominio aparece por tu nuevo requisito y debe quedar explícito.

Incluye:
- tipo de propulsión
- tipo de energía
- consumo
- autonomía estimada
- capacidad disponible
- unidades energéticas o de combustible

### Ejemplos de fuentes de energía a modelar
- gasolina
- diésel
- etanol
- energía eléctrica
- combinaciones futuras si se desea

### Variables conceptuales posibles
- litros disponibles
- densidad energética simplificada
- rendimiento del motor
- consumo por unidad de tiempo
- consumo por potencia entregada
- capacidad de batería en kWh
- energía restante

### Importancia arquitectónica
Esto no debe pegarse como un atributo suelto del bote. Debe existir como módulo o conjunto de entidades propio.

## 4.5 Subdominio de simulación
Representa la evolución temporal del sistema.

Incluye:
- estado inicial
- paso temporal
- método numérico
- trayectoria
- eventos
- magnitudes derivadas

## 4.6 Subdominio educativo
Representa la teoría integrada en la aplicación.

Incluye:
- fichas conceptuales
- fórmulas
- ejemplos
- relación teoría ↔ simulación
- categorización por tema

## 4.7 Subdominio de visualización
Representa cómo se dibuja lo que el sistema conoce.

Incluye:
- canvas 2D
- overlays
- trayectoria
- flechas de campo
- capas del escenario
- gráficas auxiliares
- posible vista estilizada 2D/2.5D/3D ligera en el futuro

## 4.8 Subdominio de persistencia y exportación
Incluye:
- guardado de escenarios
- guardado de experimentos
- exportación de imagen
- exportación de datos
- configuración del usuario

---

## 5. Decisión importante sobre 2D y 3D

## 5.1 Núcleo del proyecto
El núcleo funcional del proyecto es **2D**.

## 5.2 Sobre el 3D o estilización visual
La estilización 3D o pseudo-3D no debe alterar el dominio ni el modelo matemático.

### Regla arquitectónica
La simulación vive en 2D. Cualquier vista pseudo-3D o estilización futura será una **capa de visualización**, no un cambio del núcleo del modelo.

### Consecuencia
- el dominio no depende del render 3D
- la simulación no depende de la estética
- la vista estilizada puede añadirse más tarde sin romper el proyecto

---

## 6. Módulos principales del sistema

A continuación se define una propuesta modular concreta.

## 6.1 Módulo `app`
Responsable de:
- arranque del programa
- inicialización global
- registro de dependencias internas
- estado global de la aplicación
- navegación principal

## 6.2 Módulo `presentation`
Responsable de:
- ventana principal
- paneles
- vistas
- diálogos
- toolbar
- menús
- barra de estado
- interacción del usuario

### Submódulos sugeridos
- `main_window`
- `views`
- `panels`
- `dialogs`
- `presenters`
- `viewmodels`
- `widgets`

## 6.3 Módulo `domain`
Responsable de las entidades y reglas puras del sistema.

### Entidades esperadas
- escenario
- región
- obstáculo
- campo
- bote
- motor
- fuente de energía
- experimento
- teoría

## 6.4 Módulo `simulation`
Responsable de:
- estado dinámico
- ecuaciones de evolución
- integradores numéricos
- ejecución temporal
- cálculo de métricas derivadas

### Submódulos sugeridos
- `state`
- `models`
- `integrators`
- `runner`
- `results`
- `collision`

## 6.5 Módulo `education`
Responsable de:
- fichas teóricas
- vínculos con escenarios
- contenido textual estructurado
- metadatos educativos

## 6.6 Módulo `rendering`
Responsable de:
- dibujar el escenario
- dibujar el campo
- dibujar el bote
- dibujar overlays
- transformar coordenadas de mundo a pantalla
- visualización pseudo-3D futura si se agrega

## 6.7 Módulo `persistence`
Responsable de:
- guardar escenarios
- cargar escenarios
- guardar experimentos
- configuración
- catálogos del sistema

## 6.8 Módulo `export`
Responsable de:
- exportación de imágenes
- exportación de datos
- futuros reportes

## 6.9 Módulo `shared`
Responsable de:
- tipos comunes
- enums
- utilidades transversales
- errores comunes
- constantes del proyecto

---

## 7. Modelo de dependencias entre módulos

La idea general debe ser:

- `presentation` depende de `application`
- `application` depende de `domain`, `simulation`, `education`, `persistence`, `export`
- `simulation` depende de `domain`
- `rendering` depende de `domain` y de resultados de `simulation`
- `domain` no debe depender de `presentation`
- `domain` no debe depender de `rendering`
- `simulation` no debe depender de widgets ni de pantallas

### Regla de oro
La GUI nunca debe contener la matemática central ni las reglas del dominio.

---

## 8. Capa de aplicación y casos de uso

La capa de aplicación debe coordinar acciones concretas del usuario.

### Casos de uso esperados
- crear escenario
- cargar escenario
- guardar escenario
- colocar bote
- configurar campo
- configurar motor
- configurar fuente de energía
- iniciar simulación
- pausar simulación
- reiniciar simulación
- exportar resultado
- abrir teoría asociada

### Función de la capa
Servir como puente entre la presentación y el dominio/simulación.

---

## 9. Arquitectura interna del subdominio de propulsión y energía

Este punto es importante por el nuevo requisito de gasolina, etanol o energía eléctrica.

## 9.1 Entidades sugeridas

### `EnergySource`
Representa la fuente general de energía.

Atributos posibles:
- tipo
- unidad base
- capacidad total
- capacidad restante
- densidad o equivalencia energética simplificada

### `FuelType`
Catálogo de combustible o fuente.

Ejemplos:
- gasolina
- diésel
- etanol
- eléctrico

### `PropulsionSystem`
Representa el sistema que convierte energía disponible en empuje útil.

Atributos posibles:
- eficiencia
- potencia máxima
- tipo de fuente
- consumo base
- modelo de consumo

### `ConsumptionModel`
Define cómo se calcula el gasto.

Ejemplos:
- consumo por tiempo
- consumo por potencia instantánea
- consumo por perfil de uso

## 9.2 Responsabilidades del subdominio

- determinar si hay energía suficiente
- estimar energía o combustible restante
- calcular consumo acumulado
- relacionar potencia y gasto
- avisar si el motor ya no puede sostener cierto empuje

## 9.3 Decisión de MVP
En el MVP no debe hacerse una ingeniería de motores compleja.

Se recomienda:
- capacidad inicial
- consumo simplificado
- energía restante
- validación de suficiencia

Eso ya permite preguntas como:
- ¿alcanza con tantos litros?
- ¿cuánta energía consumió?
- ¿qué diferencia hay entre gasolina y eléctrico bajo el mismo experimento?

---

## 10. Separación entre dominio y simulación

Aunque están conectados, no son lo mismo.

## 10.1 Dominio
Responde preguntas como:
- qué es un bote
- qué es un campo
- qué es un escenario
- qué es una fuente de energía
- qué parámetros tiene un experimento

## 10.2 Simulación
Responde preguntas como:
- cómo evoluciona el estado en el tiempo
- qué integrador se usa
- qué trayectoria resulta
- cuánta energía se gastó
- qué eventos ocurrieron

### Regla
El dominio describe el mundo. La simulación lo pone en movimiento.

---

## 11. Flujo de datos principal

## 11.1 Flujo en simulación
1. el usuario modifica parámetros en la interfaz
2. la capa de presentación envía la acción a la capa de aplicación
3. la capa de aplicación construye o actualiza el experimento
4. la capa de simulación ejecuta el modelo
5. la capa de simulación devuelve resultados
6. la capa de rendering y la presentación actualizan vistas y paneles

## 11.2 Flujo en teoría
1. el usuario elige un concepto
2. la capa de aplicación consulta el módulo educativo
3. la vista muestra contenido, fórmula y ejemplo
4. si se solicita, se abre un escenario vinculado

## 11.3 Flujo en editor
1. el usuario pinta o modifica el escenario
2. la presentación genera acciones
3. la capa de aplicación actualiza el dominio del escenario
4. la vista se redibuja
5. si el usuario guarda, la persistencia serializa el escenario

---

## 12. Organización sugerida del repositorio

```text
proyecto_cpp/
  CMakeLists.txt
  CMakePresets.json
  README.md
  docs/
    PRODUCT_SPEC.md
    MATH_AND_SIMULATION.md
    UX_AND_SCREENS.md
    ARCHITECTURE.md
    BUILD_AND_DEV.md
    ROADMAP.md
    decisions/
  assets/
    icons/
    textures/
    examples/
  data/
    scenarios/
    theory/
  src/
    app/
    presentation/
    application/
    domain/
    simulation/
    education/
    rendering/
    persistence/
    export/
    shared/
  tests/
    domain/
    simulation/
    application/
    integration/
```

---

## 13. Responsabilidades por directorio

## `src/app`
Arranque y composición de la aplicación.

## `src/presentation`
Widgets, ventanas, paneles, presenters, navegación.

## `src/application`
Casos de uso y coordinación de módulos.

## `src/domain`
Entidades, reglas, objetos de valor, catálogos y validaciones conceptuales.

## `src/simulation`
Integradores, runner, estado temporal, colisión y resultados.

## `src/education`
Modelos de fichas teóricas, contenido y vínculos a ejemplos.

## `src/rendering`
Canvas y componentes de dibujo técnico.

## `src/persistence`
Carga y guardado de escenarios, experimentos y configuración.

## `src/export`
Exportación de imagen y datos.

## `src/shared`
Tipos transversales y utilidades comunes.

---

## 14. Estrategia de crecimiento

La arquitectura debe permitir crecer en fases.

## 14.1 Fase inicial
- escenario simple
- campo básico
- bote simple
- motor simple
- energía simplificada
- teoría mínima
- simulación base

## 14.2 Fase intermedia
- múltiples presets de campo
- más parámetros del bote
- consumo más interesante
- comparación entre combustibles o energía eléctrica
- mejores resultados visuales

## 14.3 Fase posterior
- estilización pseudo-3D
- múltiples vistas
- mejores overlays
- más contenido educativo
- escenarios más complejos

---

## 15. Riesgos arquitectónicos a evitar

- meter lógica matemática dentro de widgets
- pegar el consumo energético como simple variable suelta del bote
- mezclar render con simulación
- mezclar teoría con implementación numérica
- hacer un módulo “utils” gigante que absorba todo
- crear demasiadas abstracciones vacías desde el inicio
- intentar resolver 2D y 3D como si fueran dos motores paralelos

---

## 16. Decisiones arquitectónicas escritas en piedra

1. el proyecto será un **monolito modular de escritorio**
2. la arquitectura general será **por capas**
3. la presentación seguirá un enfoque cercano a **MVP / Presentation Model**
4. el núcleo funcional y matemático vivirá en **2D**
5. cualquier estilización 3D futura será una **capa de visualización**, no parte del núcleo de simulación
6. el sistema reconocerá explícitamente varios subdominios: escenario, campo, bote, propulsión/energía, simulación, educación, visualización y persistencia
7. el detalle de combustible o energía eléctrica del bote se modelará como un **subdominio de propulsión y energía**, no como simple dato decorativo
8. la GUI no contendrá reglas matemáticas ni físicas centrales
9. el dominio describirá el mundo y la simulación describirá su evolución temporal
10. la arquitectura debe priorizar claridad y crecimiento gradual, no sofisticación vacía

---

## 17. Frase oficial del documento

**La arquitectura del proyecto organiza una sola aplicación de escritorio en C++ como un monolito modular por capas, separando claramente interfaz, aplicación, dominio, simulación, teoría, visualización y persistencia, de modo que pueda convivir un laboratorio didáctico, un editor de escenarios y un modelo técnico simplificado del bote y su energía sin convertirse en un sistema caótico.**
