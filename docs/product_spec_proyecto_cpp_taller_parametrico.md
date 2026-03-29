# PRODUCT_SPEC — Proyecto de C++: Taller Paramétrico de Navegación y Campos 2D

## 1. Identificación del proyecto

### Nombre provisional
**Taller Paramétrico de Navegación y Campos 2D**

### Tipo de producto
Aplicación de escritorio en **C++** con interfaz gráfica rica, orientada a la exploración visual, didáctica y experimental de fenómenos simplificados relacionados con campos, trayectorias, fuerzas, energía y métodos numéricos en escenarios 2D editables.

### Naturaleza del producto
- didáctico
- experimental
- visual
- técnico
- de escritorio
- monolítico modular

---

## 2. Propósito del producto

La aplicación existe para permitir que un usuario:

- construya o cargue un escenario 2D simplificado
- defina condiciones matemáticas del entorno
- coloque un objeto navegable simplificado
- configure la acción de un motor o fuerza externa
- ejecute simulaciones numéricas
- observe resultados visuales y numéricos
- relacione esos resultados con teoría matemática y física integrada
- guarde y reutilice escenarios y experimentos propios

El producto no busca competir con simuladores profesionales ni con motores de videojuegos. Su razón de ser es la combinación de:

- aprendizaje
- exploración visual
- repaso conceptual
- experimentación personal
- documentación técnica seria

---

## 3. Problema que resuelve

Resolver a mano o de forma aislada ciertos problemas de movimiento, campos y trayectorias suele ser incómodo cuando se desea:

- variar parámetros rápidamente
- comparar comportamientos
- visualizar el fenómeno
- observar trayectorias y colisiones
- relacionar teoría y simulación
- guardar configuraciones reutilizables

El producto busca resolver esa incomodidad ofreciendo una herramienta donde el usuario pueda:

- configurar
- ejecutar
- observar
- comparar
- guardar
- volver a usar

sin salir del entorno de escritorio.

---

## 4. Visión general del funcionamiento

El patrón funcional base del sistema es:

**escenario + campo + objeto + fuerza/motor + método numérico → simulación → resultados**

Este flujo debe permitir desde casos simples hasta experimentos algo más ricos, pero siempre dentro de un alcance didáctico y controlado.

---

## 5. Alcance del producto

## 5.1 Dentro del alcance

La aplicación debe cubrir:

- escenarios 2D editables
- distinción entre agua, tierra y obstáculos
- representación de campos vectoriales 2D
- objeto principal navegable simplificado
- configuración de fuerza o motor
- integración numérica temporal
- cálculo de trayectoria
- colisión básica con terreno sólido
- visualización del campo y de resultados
- teoría asociada a conceptos matemáticos clave
- guardado y carga de escenarios/experimentos
- exportación básica de resultados visuales y datos

## 5.2 Fuera del alcance inicial

No forma parte de la primera versión:

- simulación realista de fluidos
- mecánica naval profesional
- elementos finitos o volúmenes finitos completos
- ray tracing
- render 3D avanzado
- mundo abierto o mapa masivo
- multijugador
- IA autónoma compleja
- scripting ilimitado
- interpretación simbólica matemática avanzada completa

---

## 6. Usuarios previstos

## 6.1 Usuario principal
El propio autor del proyecto, como:

- estudiante
- experimentador
- usuario técnico
- constructor de escenarios
- lector de teoría integrada

## 6.2 Usuario secundario hipotético
Cualquier persona interesada en:

- matemáticas aplicadas
- simulación simple
- computación gráfica básica
- software educativo técnico
- exploración visual de conceptos científicos

---

## 7. Objetivos funcionales del producto

El sistema debe permitir, como objetivos de alto nivel:

1. crear o cargar un escenario
2. definir el comportamiento del entorno mediante campos
3. colocar y configurar el objeto navegable
4. definir una fuerza o motor
5. elegir el método numérico
6. simular el comportamiento a lo largo del tiempo
7. visualizar la trayectoria y variables derivadas
8. consultar teoría relacionada
9. guardar y reutilizar configuraciones
10. exportar resultados

---

## 8. Modos principales del sistema

## 8.1 Modo teoría
Modo de consulta y aprendizaje.

Debe presentar conceptos con un tono didáctico, sobrio y visualmente explicativo, con inspiración de software educativo de escritorio clásico.

### Debe incluir
- título del concepto
- explicación breve
- fórmula o fórmulas relevantes
- interpretación intuitiva
- mini animación o visualización predeterminada
- enlace o botón para ver ese concepto en simulación

### Conceptos mínimos sugeridos
- campo vectorial
- trayectoria
- fuerza
- velocidad
- aceleración
- trabajo
- potencia
- energía
- integral de línea
- circulación
- divergencia
- rotor
- método de Euler
- Runge-Kutta

---

## 8.2 Modo simulación
Modo donde el usuario ejecuta un experimento con parámetros dados.

### Debe permitir
- cargar un escenario o plantilla
- visualizar el campo
- colocar el bote
- configurar el motor
- seleccionar método numérico
- iniciar, pausar, reiniciar y repetir simulación
- observar resultados visuales y numéricos

---

## 8.3 Modo editor
Modo donde el usuario construye o modifica escenarios y configuraciones.

### Debe permitir
- definir o pintar regiones del mapa
- colocar tierra, agua y obstáculos
- definir o superponer campos predefinidos
- guardar escenarios
- guardar experimentos completos
- duplicar escenarios para variantes

---

## 9. Entidades funcionales del sistema

## 9.1 Escenario
Representa el entorno espacial del experimento.

### Debe contener
- dimensiones del mapa
- estructura lógica del terreno
- zonas navegables
- zonas sólidas
- obstáculos
- metadatos del escenario

### Responsabilidades
- almacenar el estado del mundo 2D
- informar qué zonas son válidas o no
- servir de soporte a la visualización y a la simulación

---

## 9.2 Campo
Representa el comportamiento vectorial del entorno sobre el espacio.

### Tipos de campo mínimos
- uniforme
- lineal
- radial
- rotacional/remolino
- combinación de varios
- función definida por el usuario dentro de restricciones controladas

### Responsabilidades
- devolver el vector del campo en un punto dado
- permitir composición o superposición controlada
- alimentar la simulación y la visualización

---

## 9.3 Bote u objeto navegable
Representa el objeto móvil principal del sistema.

### Representación inicial
- rectángulo
o
- polígono rígido simple

### Debe tener
- posición
- orientación
- velocidad
- tamaño
- masa
- arrastre simplificado
- parámetros del casco simplificados si aplica

### Responsabilidades
- interactuar con el campo
- recibir empuje del motor
- participar en colisiones
- producir trayectoria y variables derivadas

---

## 9.4 Motor o fuerza controlable
Representa la acción impulsora aplicada al bote.

### Formas mínimas de configuración
- constante
- dependiente del tiempo
- por tramos
- expresada mediante una función controlada

### Debe permitir
- definir magnitud
- definir dirección relativa al bote
- variar intensidad durante la simulación

---

## 9.5 Simulación
Representa una ejecución completa del sistema bajo ciertas condiciones.

### Debe incluir
- estado inicial
- parámetros numéricos
- método de integración
- historial temporal
- eventos
- resultados agregados

---

## 10. Requerimientos funcionales detallados

## 10.1 Requerimientos del escenario

### RF-ESC-01
El usuario debe poder crear un escenario nuevo indicando dimensiones básicas.

### RF-ESC-02
El usuario debe poder marcar regiones como agua navegable.

### RF-ESC-03
El usuario debe poder marcar regiones como tierra no navegable.

### RF-ESC-04
El usuario debe poder colocar obstáculos sólidos dentro del escenario.

### RF-ESC-05
El usuario debe poder guardar un escenario para reutilizarlo.

### RF-ESC-06
El usuario debe poder cargar un escenario previamente guardado.

### RF-ESC-07
El usuario debe poder duplicar un escenario para experimentar variantes.

### RF-ESC-08
El sistema debe impedir que el bote inicie en una zona inválida.

---

## 10.2 Requerimientos del campo

### RF-CAM-01
El sistema debe incluir varios campos predefinidos listos para usar.

### RF-CAM-02
El usuario debe poder asignar un campo al escenario.

### RF-CAM-03
El usuario debe poder combinar más de un campo si la versión lo permite.

### RF-CAM-04
El sistema debe poder calcular el vector resultante del campo en cualquier punto de la zona activa.

### RF-CAM-05
El usuario debe poder visualizar el campo en forma de flechas.

### RF-CAM-06
El usuario debe poder activar o desactivar visualizaciones auxiliares del campo.

### RF-CAM-07
Idealmente, el usuario debe poder definir componentes del campo mediante expresiones matemáticas controladas.

---

## 10.3 Requerimientos del bote

### RF-BOT-01
El usuario debe poder colocar el bote en el escenario.

### RF-BOT-02
El usuario debe poder definir su orientación inicial.

### RF-BOT-03
El usuario debe poder ajustar parámetros mínimos como masa y tamaño.

### RF-BOT-04
El sistema debe actualizar la posición del bote en el tiempo durante la simulación.

### RF-BOT-05
El sistema debe calcular una trayectoria visible.

### RF-BOT-06
El sistema debe registrar colisiones con tierra u obstáculos.

### RF-BOT-07
El sistema debe impedir que el bote atraviese terreno sólido en la primera versión.

---

## 10.4 Requerimientos del motor

### RF-MOT-01
El usuario debe poder definir una magnitud de empuje constante.

### RF-MOT-02
El usuario debe poder definir una función de empuje simple dependiente del tiempo o por tramos.

### RF-MOT-03
El sistema debe permitir orientar el empuje respecto al eje del bote.

### RF-MOT-04
El sistema debe poder calcular potencia instantánea y energía acumulada, al menos en modelos donde esto tenga sentido.

### RF-MOT-05
Las funciones definidas por el usuario deben poder mostrarse como fórmula legible.

---

## 10.5 Requerimientos de simulación numérica

### RF-SIM-01
El usuario debe poder iniciar una simulación desde un estado configurado.

### RF-SIM-02
El usuario debe poder pausar y reiniciar la simulación.

### RF-SIM-03
El sistema debe ofrecer al menos dos métodos numéricos en etapas iniciales recomendadas.

### RF-SIM-04
El usuario debe poder elegir el paso temporal.

### RF-SIM-05
El sistema debe almacenar la evolución temporal del estado.

### RF-SIM-06
El sistema debe permitir repetir la simulación con parámetros distintos.

### RF-SIM-07
El sistema debe calcular y mostrar variables derivadas relevantes.

---

## 10.6 Requerimientos de teoría

### RF-TEO-01
El sistema debe incluir fichas teóricas de conceptos seleccionados.

### RF-TEO-02
Cada ficha debe contener explicación breve y fórmula.

### RF-TEO-03
Cada ficha debe incluir una visualización o ejemplo mínimo.

### RF-TEO-04
Cuando sea posible, el sistema debe enlazar una ficha teórica con un escenario demostrativo.

### RF-TEO-05
Cuando exista una solución analítica simple, el sistema debe poder mostrarla o al menos referenciarla junto con la aproximación numérica.

---

## 10.7 Requerimientos de visualización

### RF-VIS-01
El sistema debe mostrar el escenario 2D completo.

### RF-VIS-02
El sistema debe mostrar el campo vectorial sobre el escenario.

### RF-VIS-03
El sistema debe mostrar el bote y su orientación.

### RF-VIS-04
El sistema debe mostrar la trayectoria recorrida.

### RF-VIS-05
El sistema debe poder mostrar capas opcionales, como flechas, líneas o zonas resaltadas.

### RF-VIS-06
El sistema debe incluir paneles o pestañas con resultados numéricos y gráficas auxiliares si la versión lo soporta.

---

## 10.8 Requerimientos de guardado y exportación

### RF-SAV-01
El usuario debe poder guardar escenarios.

### RF-SAV-02
El usuario debe poder guardar experimentos completos.

### RF-SAV-03
El sistema debe permitir cargar escenarios o experimentos guardados.

### RF-SAV-04
El usuario debe poder exportar una imagen del escenario o del resultado.

### RF-SAV-05
El usuario debe poder exportar datos numéricos básicos de la simulación.

---

## 11. Requerimientos de interacción del usuario

El flujo principal esperado debe ser:

1. abrir la aplicación
2. elegir teoría, simulación o editor
3. abrir o crear escenario
4. configurar campo y bote
5. definir el motor
6. elegir método numérico
7. ejecutar simulación
8. observar resultados
9. ajustar parámetros
10. guardar o exportar

El sistema debe minimizar fricción en ese ciclo de experimentación.

---

## 12. Reglas del sistema

## 12.1 Reglas de espacio
- el bote no puede iniciar en tierra
- los obstáculos son sólidos
- el mapa debe tener al menos una zona navegable válida para simular

## 12.2 Reglas de campo
- los campos deben evaluarse de forma coherente dentro de los límites numéricos definidos
- el sistema debe manejar valores fuera de rango de forma segura

## 12.3 Reglas de motor
- el motor no puede aplicar valores físicamente absurdos sin advertencia si el sistema establece límites
- el usuario debe conocer qué función de empuje está activa

## 12.4 Reglas de simulación
- el método numérico elegido debe quedar registrado en el experimento
- el paso temporal debe influir claramente en la precisión y en el resultado
- las colisiones deben registrarse como evento

---

## 13. Requerimientos de experiencia visual

La aplicación debe tener una sensación de:

- laboratorio interactivo
- software técnico clásico
- sistema educativo tipo museo digital
- utilidad de escritorio rica pero sobria

### Elementos visuales esperados
- ventana principal con menú
- toolbar o barra de acciones
- panel lateral o superior de parámetros
- canvas central de visualización
- inspector o panel de propiedades
- pestañas inferiores o laterales para teoría, datos y resultados
- barra de estado

### Referencia estilística
Inspiración indirecta en software técnico/educativo de escritorio de la era 2000–2010, con énfasis en claridad, paneles y navegación evidente.

---

## 14. Requerimientos educativos

El sistema no debe limitarse a “mostrar un resultado”. Debe ayudar a entender.

Por ello, debe buscar:

- lenguaje explicativo corto
- relación entre fórmula y fenómeno
- mini demostraciones predeterminadas
- escenarios diseñados para enseñar un concepto
- comparación entre intuición visual y cálculo

---

## 15. Requerimientos de configurabilidad

El usuario debe poder configurar, según la etapa del proyecto:

- tamaño del escenario
- tipo y parámetros del campo
- posición inicial del bote
- orientación inicial
- masa/tamaño del bote
- motor
- método numérico
- duración de la simulación
- paso de tiempo
- capas de visualización activas

---

## 16. Requerimientos de extensibilidad

Aunque la primera versión será acotada, el producto debe dejar espacio razonable para incorporar después:

- nuevos tipos de campo
- nuevos objetos navegables
- nuevos escenarios predeterminados
- más teoría integrada
- más métodos numéricos
- reportes o exportaciones más completas
- controles más finos del modelo físico

---

## 17. Requerimientos no funcionales

## 17.1 Claridad
El código y el comportamiento del sistema deben ser comprensibles para quien estudia el proyecto.

## 17.2 Mantenibilidad
La solución debe organizarse de modo que futuras extensiones no destruyan la base.

## 17.3 Reproducibilidad
El proyecto debe poder compilarse y ejecutarse de manera razonablemente estable siguiendo la documentación.

## 17.4 Trazabilidad
Las decisiones importantes del proyecto deben quedar registradas en documentos.

## 17.5 Sobriedad visual
La aplicación puede ser visualmente rica, pero no debe depender de efectos recargados para justificarse.

---

## 18. Escenarios de ejemplo mínimos

La primera versión funcional debería incluir al menos algunos escenarios demostrativos predefinidos.

### Escenario A
Río con corriente uniforme.

### Escenario B
Remolino localizado.

### Escenario C
Canal con obstáculo.

### Escenario D
Campo mixto con zona lenta y zona rápida.

Cada uno debe servir para explicar al menos una idea matemática o física.

---

## 19. Límites del MVP

Para proteger el alcance, el MVP debe limitarse a:

- 2D
- un solo objeto navegable principal
- terreno simple por regiones o celdas
- campo vectorial simplificado
- colisión básica
- métodos numéricos básicos seleccionados
- teoría mínima integrada
- guardado/carga simple

No debe intentar desde la primera etapa:

- simulación total del medio
- geometrías demasiado complejas
- scripting libre avanzado
- 3D realista
- interpretación simbólica avanzada

---

## 20. Definición del MVP

El producto mínimo viable se considerará alcanzado si permite:

- crear o cargar un escenario 2D
- representar agua y tierra
- asignar un campo vectorial predefinido
- colocar un bote rectangular simplificado
- configurar un motor básico
- elegir un método numérico simple
- ejecutar la simulación
- ver trayectoria y colisión básica
- consultar una ficha teórica asociada
- guardar/cargar un escenario sencillo

---

## 21. Criterios de éxito del producto

El producto se considerará bien orientado si logra:

- enseñar algo real al usuario
- permitir experimentar sin fricción excesiva
- conectar teoría y práctica visual
- sostener una GUI rica sin caos estructural
- ser presentable como proyecto serio de C++ en GitHub
- dejar una base ampliable sin rehacer todo

---

## 22. Riesgos funcionales a evitar

- meter demasiada física realista demasiado pronto
- intentar resolver toda interacción del agua con el casco de forma avanzada en la primera etapa
- sobrecargar el editor de terreno antes de estabilizar la simulación básica
- convertir el modo teoría en un texto largo sin valor visual
- depender de fórmulas libres sin control ni validación
- mezclar teoría, simulación y edición sin una navegación clara

---

## 23. Decisiones funcionales escritas en piedra

1. el proyecto será una **sola aplicación de escritorio en C++**
2. el producto será **didáctico y experimental**, no profesional-industrial
3. la simulación será inicialmente **2D y simplificada**
4. el usuario podrá **crear, guardar y reutilizar escenarios**
5. la app incluirá un **modo teoría** además del modo simulación/editor
6. la app integrará **campos, trayectorias, energía y métodos numéricos** como núcleo conceptual
7. el alcance inicial evitará física de fluidos compleja y renderizado avanzado
8. el MVP priorizará **claridad funcional** sobre complejidad visual extrema

---

## 24. Frase oficial del producto

**Aplicación de escritorio en C++ para la exploración didáctica de campos vectoriales, trayectorias, energía y métodos numéricos en escenarios 2D editables, con integración de teoría, simulación y experimentación visual.**
