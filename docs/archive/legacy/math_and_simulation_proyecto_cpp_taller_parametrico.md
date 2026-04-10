# MATH_AND_SIMULATION — Proyecto de C++: Taller Paramétrico de Navegación y Campos 2D

## 1. Propósito del documento

Este documento fija la base matemática y de simulación del proyecto.

Su objetivo es dejar claro:

- qué fenómenos se desean representar
- qué simplificaciones se aceptan
- qué variables existen en el modelo
- qué parte puede resolverse analíticamente
- qué parte se resolverá numéricamente
- qué métodos numéricos se usarán
- qué conceptos matemáticos se quieren enseñar

Este documento no define la arquitectura del software ni la interfaz. Solo define el corazón matemático y físico simplificado del sistema.

---

## 2. Filosofía del modelo

La aplicación no busca una simulación profesional de fluidos ni de navegación realista.

El modelo debe ser:

- didáctico
- controlado
- visualmente interesante
- razonablemente coherente
- suficientemente simple para implementarse y entenderse
- suficientemente rico para ilustrar conceptos reales

La filosofía base es:

**usar modelos simplificados, pero honestos**.

Eso significa:

- no fingir precisión profesional donde no la hay
- explicar cuándo un resultado es aproximado
- distinguir entre solución analítica y numérica
- dejar claras las suposiciones del modelo

---

## 3. Patrón general del problema

El sistema representa un objeto navegable simplificado en un entorno bidimensional donde actúan:

- un campo vectorial del medio
- una fuerza o motor controlable
- un posible rozamiento o arrastre simplificado
- restricciones espaciales del escenario

El flujo conceptual es:

**estado inicial + modelo del entorno + fuerza aplicada + método numérico → evolución temporal**

---

## 4. Dimensionalidad y alcance matemático

## 4.1 Dimensión inicial
La simulación base será en **2D**.

### Justificación
- reduce complejidad
- permite visualización clara
- sigue siendo suficientemente rica para ilustrar cálculo vectorial, métodos numéricos y física básica
- facilita un MVP alcanzable

## 4.2 Espacio del escenario
El escenario se considerará inicialmente una región del plano.

Punto genérico:

\[
\mathbf{r}(t) = (x(t), y(t))
\]

## 4.3 Tiempo
La simulación es dependiente del tiempo.

\[
t \in [0, T]
\]

---

## 5. Variables del sistema

## 5.1 Variables de estado del bote
El bote u objeto móvil simplificado tendrá, como mínimo:

- posición: \(x, y\)
- velocidad: \(v_x, v_y\)
- orientación: \(\theta\) si se modela
- velocidad angular: opcional en fases posteriores

Un estado base razonable es:

\[
\mathbf{s}(t) = (x(t), y(t), v_x(t), v_y(t))
\]

En una versión más rica:

\[
\mathbf{s}(t) = (x(t), y(t), v_x(t), v_y(t), \theta(t), \omega(t))
\]

## 5.2 Variables del entorno

- campo vectorial del medio: \(\mathbf{F}(x,y,t)\) o \(\mathbf{U}(x,y)\)
- mapa del escenario
- regiones navegables y no navegables
- parámetros de arrastre

## 5.3 Variables del motor

- magnitud de empuje
- dirección del empuje
- posible dependencia temporal

Ejemplo:

\[
\mathbf{T}(t) = T(t)\,\hat{u}(t)
\]

---

## 6. Campo vectorial del entorno

## 6.1 Interpretación
El campo vectorial representa el comportamiento local del medio en cada punto del plano.

Ejemplo general:

\[
\mathbf{U}(x,y) = (u(x,y), v(x,y))
\]

Esto puede interpretarse como:

- velocidad del agua
- intensidad y dirección del flujo
- un campo externo que influye el movimiento

## 6.2 Tipos de campo deseados

### Campo uniforme
\[
\mathbf{U}(x,y) = (a, b)
\]

**Uso didáctico:** corriente constante.

### Campo lineal
\[
\mathbf{U}(x,y) = (ax + by, cx + dy)
\]

**Uso didáctico:** variación espacial simple.

### Campo radial
\[
\mathbf{U}(x,y) = k(x-x_0, y-y_0)
\]

**Uso didáctico:** fuente o sumidero simplificado.

### Campo rotacional / remolino
Ejemplo simple:

\[
\mathbf{U}(x,y) = \big(-(y-y_0),\; x-x_0\big)
\]

**Uso didáctico:** circulación y rotor.

### Campo compuesto
Suma de varios campos:

\[
\mathbf{U}_{\text{total}} = \mathbf{U}_1 + \mathbf{U}_2 + \cdots + \mathbf{U}_n
\]

**Uso didáctico:** entornos más ricos con zonas mixtas.

---

## 7. Modelo del bote

## 7.1 Representación geométrica
En la primera etapa, el bote no será una malla física compleja.

Se modelará como:

- un rectángulo
o
- un polígono rígido simple

La geometría se usará principalmente para:

- visualización
- orientación
- detección básica de colisión
- futuros puntos de muestreo sobre el casco

## 7.2 Aproximación física inicial
La interacción del bote con el entorno se simplifica a nivel de partícula o cuerpo rígido simple.

### Modelo más simple
Se usa solo la posición del centro y la velocidad.

### Modelo intermedio
Se usan varios puntos de muestreo del casco para estimar interacción y posible torque, pero esto queda para una etapa posterior.

---

## 8. Fuerzas consideradas

## 8.1 Empuje del motor
El motor aplica una fuerza controlable.

Forma general:

\[
\mathbf{T}(t) = T(t)\,\hat{d}(t)
\]

Donde:

- \(T(t)\): magnitud del empuje
- \(\hat{d}(t)\): dirección del empuje

## 8.2 Arrastre o rozamiento simplificado
Se puede usar un término de oposición al movimiento relativo.

Ejemplo lineal simple:

\[
\mathbf{D} = -c\,\mathbf{v}
\]

O relativo al medio:

\[
\mathbf{D} = -c\,(\mathbf{v} - \mathbf{U}(x,y))
\]

## 8.3 Fuerza neta
Un modelo simplificado razonable sería:

\[
\mathbf{F}_{\text{net}} = \mathbf{T} + \mathbf{F}_{\text{medio}} + \mathbf{D}
\]

Dependiendo del enfoque, el campo del medio puede influir:

- como velocidad local del fluido
- como fuerza efectiva simplificada

Estas dos interpretaciones no deben mezclarse sin cuidado. El proyecto debe escoger una por escenario o por modo.

---

## 9. Dos enfoques físicos posibles

Aquí hay una decisión importante.

## 9.1 Enfoque A — El campo representa velocidad del medio
En este caso, el agua tiene una velocidad local \(\mathbf{U}(x,y)\).

El bote se mueve según:

- su empuje
- su velocidad relativa al medio
- arrastre relativo

Este enfoque es más intuitivo para el caso del río.

## 9.2 Enfoque B — El campo representa fuerza externa
En este caso, el campo ya actúa como fuerza directa sobre el objeto.

\[
m\,\mathbf{a} = \mathbf{T}(t) + \mathbf{U}(x,y) + \mathbf{D}
\]

Este enfoque es más simple algebraicamente, pero menos fiel a la intuición de “agua con velocidad”.

## 9.3 Decisión recomendada
Para el proyecto, el enfoque preferible es:

**el campo como velocidad local del medio**

porque se alinea mejor con la narrativa de río, lago y remolinos.

---

## 10. Modelo dinámico base

Un modelo simplificado y útil para la primera versión puede escribirse así:

\[
\frac{d\mathbf{r}}{dt} = \mathbf{v}
\]

\[
m\frac{d\mathbf{v}}{dt} = \mathbf{T}(t) - c(\mathbf{v} - \mathbf{U}(x,y))
\]

Donde:

- \(\mathbf{r}(t)\): posición
- \(\mathbf{v}(t)\): velocidad del bote
- \(m\): masa
- \(c\): coeficiente de arrastre
- \(\mathbf{U}(x,y)\): velocidad del medio
- \(\mathbf{T}(t)\): empuje del motor

Este sistema ya permite:

- trayectorias interesantes
- influencia del medio
- efecto del motor
- gasto energético
- integración numérica clara

---

## 11. Casos con solución analítica simple

Es importante distinguir qué escenarios podrían tener solución cerrada o al menos comparación analítica.

## 11.1 Caso simple 1
Campo uniforme constante, motor constante, sin colisiones.

En algunos casos se puede obtener una solución explícita o al menos una forma cerrada manejable.

## 11.2 Caso simple 2
Movimiento sin arrastre y con fuerza constante.

Es equivalente a un movimiento uniformemente acelerado vectorial simplificado.

## 11.3 Caso simple 3
Movimiento sin motor, arrastrado solo por un campo constante.

Sirve para comparar intuición visual con formulación matemática.

## 11.4 Decisión didáctica
Siempre que exista un caso simple con solución analítica razonable, el sistema debería poder:

- mostrar la forma analítica
- graficarla o visualizarla
- compararla con la numérica

---

## 12. Casos donde se requiere método numérico

Será necesario usar métodos numéricos cuando:

- el campo dependa de la posición de forma no trivial
- el motor dependa del tiempo o del estado
- exista arrastre no lineal o interacción más rica
- el escenario tenga obstáculos y se requiera paso temporal
- no exista solución cerrada sencilla

Aquí el valor didáctico del programa es muy alto, porque justamente permite ver por qué “no queda de otra” y hay que aproximar.

---

## 13. Métodos numéricos previstos

## 13.1 Euler explícito
Fórmula general para una EDO:

\[
\mathbf{s}_{n+1} = \mathbf{s}_n + h\,f(t_n, \mathbf{s}_n)
\]

### Ventajas
- simple
- muy didáctico
- fácil de implementar

### Desventajas
- poco preciso
- puede acumular error rápido

### Uso en el proyecto
- método inicial obligatorio
- comparación base

---

## 13.2 Euler mejorado / Heun
Forma conceptual:

- predicción con Euler
- corrección con promedio de pendientes

### Ventajas
- mejora sobre Euler
- sigue siendo comprensible

### Uso en el proyecto
- opcional en etapa temprana o intermedia

---

## 13.3 Runge–Kutta de orden 4 (RK4)
Método clásico de mayor precisión para muchos problemas didácticos.

### Ventajas
- bastante preciso para muchos escenarios
- ampliamente conocido
- excelente para comparación educativa

### Desventajas
- más costoso
- más complejo de explicar que Euler

### Uso en el proyecto
- método recomendado de referencia para la primera etapa robusta

---

## 14. Variables derivadas a calcular

Durante o después de la simulación, el sistema debe poder calcular:

- trayectoria
- velocidad
- aceleración estimada
- tiempo total
- distancia recorrida
- potencia instantánea
- energía acumulada
- eventos de colisión

## 14.1 Potencia
Si el empuje del motor es \(\mathbf{T}\) y la velocidad del bote es \(\mathbf{v}\):

\[
P(t) = \mathbf{T}(t) \cdot \mathbf{v}(t)
\]

## 14.2 Energía consumida o trabajo acumulado

\[
E = \int_0^T P(t)\,dt
\]

Numéricamente:

\[
E \approx \sum_n P(t_n)\,h
\]

Esto conecta cálculo, física y métodos numéricos de forma muy bonita.

---

## 15. Colisiones y restricciones espaciales

## 15.1 Naturaleza de la colisión inicial
En el MVP, la colisión será simple:

- si el bote entra en terreno inválido, se detecta colisión
- el movimiento se detiene o se corrige de forma básica

## 15.2 Propósito
El objetivo no es una física de colisión compleja, sino:

- impedir atravesar tierra
- registrar un evento relevante
- enriquecer la experimentación

## 15.3 Reglas espaciales
- agua: región navegable
- tierra: región no navegable
- obstáculo: región sólida

---

## 16. Conceptos matemáticos que el sistema quiere enseñar

## 16.1 Cálculo de una variable

### Conceptos
- tasa de cambio
- acumulación
- potencia
- energía como integral temporal

### Microidea
“Cómo cambia una cantidad y cuánto se acumula.”

---

## 16.2 Cálculo multivariable

### Conceptos
- campo vectorial
- campo escalar
- gradiente
- divergencia
- rotor
- integral de línea
- circulación

### Microidea
“En cada punto del espacio puede haber un valor o una flecha, y eso cambia cómo se mueve el objeto.”

---

## 16.3 Álgebra lineal

### Conceptos
- vectores
- matrices
- proyecciones
- rotación en 2D
- base local del bote
- descomposición en componentes

### Microidea
“Los vectores y matrices permiten describir dirección, transformación y movimiento.”

---

## 16.4 Física simplificada

### Conceptos
- fuerza
- empuje
- arrastre
- velocidad relativa
- trabajo
- potencia
- energía

### Microidea
“El objeto se mueve porque se combinan empuje, medio y oposición al movimiento.”

---

## 16.5 Métodos numéricos

### Conceptos
- discretización temporal
- error de aproximación
- estabilidad básica
- comparación entre métodos

### Microidea
“Cuando no se puede resolver bonito, se aproxima paso a paso.”

---

## 17. Teoremas y herramientas conceptuales vinculables

No todos deben implementarse profundamente, pero sí pueden aparecer en modo teoría.

## 17.1 Teorema de Green
Muy relevante para conectar integrales de línea con integrales dobles en el plano.

### Uso didáctico
- circulación
- flujo en regiones planas
- conexión entre borde y región

## 17.2 Campo conservativo
Puede aparecer en escenarios o explicaciones donde un campo derive de un potencial.

## 17.3 Potencial escalar
Útil si luego se exploran gradientes y navegación influida por campos escalares.

---

## 18. Expresiones matemáticas del usuario

El sistema idealmente permitirá definir expresiones controladas para:

- componentes del campo
- magnitud del motor
- funciones temporales simples

Ejemplos:

- \(u(x,y) = 2\)
- \(v(x,y) = \sin(x)\)
- \(T(t) = 5 + 2\sin(t)\)

## 18.1 Restricción importante
No se busca crear un CAS completo (Computer Algebra System).

La evaluación de expresiones debe ser:

- limitada
- segura
- controlada
- pensada para escenarios educativos

---

## 19. Resultados visuales deseados

La matemática debe tener traducción visual.

El sistema debe poder mostrar, según el escenario:

- flechas del campo
- líneas de flujo
- trayectoria del bote
- orientación del bote
- zonas con color por intensidad
- curvas o gráficas derivadas
- comparación entre simulaciones

El objetivo es que el usuario no solo lea ecuaciones, sino que vea el fenómeno.

---

## 20. Tipos de escenarios didácticos recomendados

## Escenario 1
**Corriente uniforme**

Enseña:
- campo constante
- trayectoria simple
- comparación analítica/numerica

## Escenario 2
**Remolino aislado**

Enseña:
- rotor
- circulación
- sensibilidad a la posición inicial

## Escenario 3
**Canal con obstáculo**

Enseña:
- restricciones espaciales
- trayectoria condicionada
- importancia del control del motor

## Escenario 4
**Zona lenta / zona rápida**

Enseña:
- campo compuesto
- optimización intuitiva de la ruta

## Escenario 5
**Motor variable en el tiempo**

Enseña:
- función temporal
- potencia y energía acumulada

---

## 21. Reglas de honestidad del modelo

Para que el proyecto siga siendo intelectualmente limpio, se adoptan estas reglas:

1. si algo es una aproximación, debe asumirse como tal
2. si un escenario es puramente didáctico, no debe venderse como realista
3. si existe solución analítica y se usa aproximación, debe explicarse por qué
4. si el método numérico introduce error visible, eso puede convertirse en material didáctico
5. la matemática integrada debe servir para entender, no para decorar

---

## 22. Prioridades matemáticas del MVP

Para evitar dispersión, el MVP debe centrarse en:

- campo vectorial 2D
- estado posición–velocidad
- motor básico
- arrastre lineal simple
- Euler y RK4
- trayectoria
- potencia y energía acumulada
- colisión básica con terreno

Eso ya es suficientemente rico para enseñar bastante.

---

## 23. Qué queda para etapas futuras

No debe entrar en la primera etapa:

- interacción fluido–casco detallada
- mallado del bote
- turbulencia
- modelos de agua físicamente realistas
- acoplamientos complejos entre orientación y medio
- optimización automática sofisticada de ruta
- métodos avanzados de PDE completos

---

## 24. Decisiones matemáticas escritas en piedra

1. la simulación base será **2D**
2. el bote será inicialmente un **objeto rígido simple**, no una malla física compleja
3. el entorno se modelará con un **campo vectorial del medio**
4. el sistema dinámico se resolverá **numéricamente** cuando no exista una forma simple analítica
5. los métodos mínimos previstos serán **Euler** y **RK4**
6. el sistema calculará al menos **trayectoria, potencia y energía acumulada**
7. la matemática integrada deberá ser **didáctica, no ornamental**
8. el enfoque inicial preferido es interpretar el campo como **velocidad local del medio**

---

## 25. Frase oficial del documento

**El proyecto modela, de forma simplificada y didáctica, el movimiento de un objeto navegable en un entorno 2D afectado por un campo vectorial y por una fuerza controlable, resolviendo la evolución temporal mediante métodos numéricos y vinculando cada resultado con conceptos matemáticos y físicos explicables.**
