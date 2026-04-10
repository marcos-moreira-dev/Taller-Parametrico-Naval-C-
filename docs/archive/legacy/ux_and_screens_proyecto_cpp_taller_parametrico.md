# UX_AND_SCREENS — Proyecto de C++: Taller Paramétrico de Navegación y Campos 2D

## 1. Propósito del documento

Este documento define la experiencia visual y la organización de pantallas del proyecto.

Su objetivo es dejar claro:

- cómo debe sentirse la aplicación
- qué pantallas existen
- qué paneles componen la interfaz
- cómo fluye el usuario entre teoría, simulación y edición
- cómo debe organizarse la información en pantalla
- qué estilo visual de escritorio se busca

Este documento no define el detalle interno de clases ni la arquitectura del código. Se concentra en la experiencia del usuario y en la estructura de la interfaz.

---

## 2. Identidad visual buscada

La aplicación debe sentirse como una mezcla entre:

- laboratorio técnico interactivo
- software de escritorio clásico
- herramienta educativa con aire Encarta técnico
- aplicación rica en paneles, pero sobria

### Sensaciones que se quieren transmitir

- claridad
- control
- curiosidad
- precisión técnica
- exploración guiada
- nostalgia por software didáctico y de oficina 2000–2010

### Sensaciones que no se quieren transmitir

- videojuego arcade
- dashboard web moderno
- app minimalista contemporánea
- software infantil
- interfaz recargada de efectos sin propósito

---

## 3. Principios de experiencia de usuario

## 3.1 El usuario debe orientarse rápido
El usuario debe entender visualmente dónde está:

- teoría
- simulación
- editor
- resultados

## 3.2 La información debe tener jerarquía
Lo más importante debe verse primero:

- el escenario o canvas principal
- el objeto y su trayectoria
- el campo visible
- los controles relevantes

## 3.3 Lo técnico debe seguir siendo legible
Aunque la app sea rica visualmente, no debe convertirse en una pared de paneles caóticos.

## 3.4 La exploración debe ser corta de ida y vuelta
El usuario debe poder:

1. cambiar algo
2. volver a correr
3. ver qué pasó
4. comparar

sin demasiada fricción.

## 3.5 La teoría debe sentirse vinculada, no aislada
La teoría no debe parecer un PDF pegado. Debe sentirse conectada con la simulación y el escenario.

---

## 4. Estilo de escritorio de referencia

La interfaz debe inspirarse en patrones típicos de software técnico y de oficina entre 2005 y 2010.

### Elementos característicos a tomar

- ventana principal con menú superior
- toolbar con acciones frecuentes
- panel lateral de navegación o herramientas
- canvas central dominante
- panel derecho de propiedades o parámetros
- pestañas inferiores o laterales para resultados
- barra de estado
- diálogos modales clásicos

### Resultado esperado
Una aplicación que se vea como una “estación de trabajo” pequeña, no como una sola pantalla suelta.

---

## 5. Estructura global de la ventana principal

La app debe tener una **ventana principal única** que actúe como contenedor del trabajo del usuario.

## 5.1 Elementos principales de la ventana

### Barra de título
Debe mostrar:
- nombre del proyecto
- nombre del escenario o experimento actual si existe
- marca visual de cambios no guardados si aplica

### Menú superior
Debe contener al menos:
- Archivo
- Ver
- Simulación
- Teoría
- Herramientas
- Ayuda

### Toolbar principal
Debe incluir accesos rápidos a operaciones frecuentes.

### Área central de trabajo
Es el corazón visual de la aplicación.

### Barra de estado
Debe mostrar mensajes breves sobre:
- estado del escenario
- coordenadas del cursor si aplica
- modo actual
- advertencias cortas
- estado de simulación

---

## 6. Distribución principal recomendada

La distribución más coherente para este proyecto es una interfaz multipanel tipo “todo en uno”.

## 6.1 Layout general propuesto

### Panel izquierdo
Función:
- navegación entre modo teoría, editor y simulación
- biblioteca de escenarios
- capas o elementos del escenario
- lista de presets o experimentos guardados

### Panel central
Función:
- canvas principal del escenario
- visualización del bote
- visualización del campo
- trayectoria
- overlays visuales

### Panel derecho
Función:
- inspector de propiedades
- parámetros del escenario
- parámetros del bote
- parámetros del motor
- parámetros del campo
- parámetros del método numérico

### Panel inferior o pestañas inferiores
Función:
- resultados numéricos
- gráficas
- eventos
- teoría relacionada
- mensajes del sistema o logs didácticos

---

## 7. Modos visuales principales

La aplicación debe tener al menos tres modos fuertes.

## 7.1 Modo teoría
Este modo presenta conceptos de manera guiada.

### Objetivo
Enseñar conceptos matemáticos y físicos vinculados con la simulación.

### Sensación buscada
Tipo ficha técnica de museo digital o Encarta técnica.

### Estructura visual sugerida
- título del concepto
- bloque breve de explicación
- fórmula destacada
- ilustración o mini animación
- botones como:
  - ver ejemplo
  - abrir escenario asociado
  - comparar con simulación

### Contenido visible sugerido
- nombre del concepto
- idea corta
- representación gráfica
- fórmula en formato legible
- nota contextual breve

### Navegación sugerida
Conceptos organizados por categorías:
- cálculo de una variable
- cálculo multivariable
- álgebra lineal
- física simplificada
- métodos numéricos

---

## 7.2 Modo simulación
Este modo es la zona donde se ejecuta el experimento.

### Objetivo
Permitir configurar y observar el comportamiento del sistema.

### Componentes visibles esperados
- canvas principal grande
- panel de parámetros
- controles de reproducción
- resultados principales
- visualización del campo
- trayectoria del bote

### Controles principales
- iniciar
- pausar
- reiniciar
- paso a paso (si se implementa)
- cambiar velocidad de animación
- cambiar método numérico
- alternar capas visuales

### Sensación buscada
Tipo laboratorio interactivo.

---

## 7.3 Modo editor
Este modo permite construir o modificar escenarios.

### Objetivo
Diseñar mapas y experimentos reutilizables.

### Componentes visibles esperados
- canvas editable
- herramientas de pintura o selección
- lista de tipos de celda o región
- herramientas para ubicar obstáculos
- herramientas para colocar elementos decorativos simples
- configuración del campo o zonas especiales

### Sensación buscada
Tipo editor técnico sencillo, no editor artístico avanzado.

---

## 8. Flujo principal del usuario

El flujo más importante de toda la app debe poder hacerse sin perderse.

## 8.1 Flujo A — Explorar teoría y luego probar
1. abrir aplicación
2. entrar a teoría
3. leer una ficha
4. abrir escenario asociado
5. ejecutar simulación
6. cambiar parámetros
7. volver a teoría o guardar experimento

## 8.2 Flujo B — Crear un escenario desde cero
1. abrir aplicación
2. entrar al editor
3. definir agua, tierra y obstáculos
4. definir campo
5. colocar bote
6. pasar a simulación
7. ejecutar
8. guardar escenario

## 8.3 Flujo C — Abrir experimento y comparar
1. abrir escenario guardado
2. ejecutar con método A
3. guardar resultados o visualizar
4. cambiar a método B
5. comparar diferencias

---

## 9. Pantallas o áreas concretas

## 9.1 Pantalla o vista de inicio
La aplicación puede abrir mostrando una pantalla inicial o un panel de bienvenida dentro de la ventana principal.

### Debe ofrecer
- abrir escenario reciente
- crear escenario nuevo
- abrir teoría
- ver ejemplos predeterminados
- continuar último experimento

### Sensación buscada
Una bienvenida sobria y útil, no ornamental.

---

## 9.2 Vista principal de simulación
Debe ser la pantalla más importante del proyecto.

### Zona central
Canvas del escenario con:
- agua
- tierra
- obstáculos
- bote
- campo vectorial
- trayectoria
- overlays activos

### Zona superior o toolbar contextual
- iniciar
- pausar
- reiniciar
- elegir método
- exportar imagen
- alternar overlays

### Zona derecha
Inspector del experimento actual:
- parámetros del motor
- parámetros del bote
- parámetros del campo
- paso temporal
- duración
- masa
- arrastre

### Zona inferior
Pestañas como:
- resultados
- gráficas
- eventos
- teoría vinculada
- diferencias entre métodos

---

## 9.3 Vista de teoría
Puede ser una vista completa o una pestaña grande dentro del espacio central.

### Componentes sugeridos
- índice lateral de conceptos
- contenido principal de la ficha
- visual pequeña o animación
- bloque de fórmulas
- botones de navegación contextual

### Flujo ideal
El usuario lee y desde ahí puede ir directo a una demostración o simulación relacionada.

---

## 9.4 Vista de editor de escenario
Debe sentirse más como editor técnico que como herramienta de dibujo libre.

### Herramientas mínimas sugeridas
- pintar agua
- pintar tierra
- colocar obstáculo
- borrar
- relleno simple
- colocar bote inicial
- seleccionar región

### Propiedades editables
- tamaño del escenario
- nombre del escenario
- campo asociado
- restricciones especiales si las hubiera

### Ayuda visual
Conviene mostrar grilla opcional.

---

## 10. Panel izquierdo

El panel izquierdo debe ser multipropósito, pero no caótico.

## 10.1 Posibles pestañas del panel izquierdo
- Navegación
- Escenarios
- Capas
- Teoría
- Biblioteca

## 10.2 Contenidos recomendados

### Navegación
- Inicio
- Teoría
- Simulación
- Editor
- Resultados

### Escenarios
- escenarios guardados
- ejemplos predeterminados
- recientes

### Capas
- agua
- tierra
- obstáculos
- campo
- trayectoria
- overlays

### Teoría
Índice de conceptos.

---

## 11. Panel derecho: inspector de propiedades

Este panel es clave para dar sensación de software técnico serio.

## 11.1 Debe mostrar contextualmente
Según lo seleccionado, puede mostrar:

### Si está seleccionado el bote
- masa
- tamaño
- orientación inicial
- arrastre
- posición inicial

### Si está seleccionado el motor
- magnitud
- función temporal
- dirección
- límites

### Si está seleccionado el campo
- tipo
- parámetros
- intensidad
- centro si es radial o rotacional

### Si está seleccionado el escenario
- dimensiones
- nombre
- metadatos

## 11.2 Estilo esperado
Formulario técnico con etiquetas claras, muy legible.

---

## 12. Panel inferior: pestañas de resultados

Este panel ayuda a que la app se sienta rica sin invadir el canvas principal.

## 12.1 Pestañas sugeridas

### Resultados
- tiempo transcurrido
- energía consumida
- colisiones
- distancia recorrida

### Gráficas
- posición vs tiempo
- velocidad vs tiempo
- potencia vs tiempo
- energía acumulada

### Eventos
- colisión detectada
- inicio de simulación
- fin de simulación
- advertencias

### Teoría vinculada
- resumen corto del concepto activo
- fórmulas relacionadas
- acceso rápido a ficha completa

### Comparación
- método A vs método B
- diferencias visibles
- error cualitativo o cuantitativo si se implementa

---

## 13. Menús de la aplicación

## 13.1 Menú Archivo
Debe incluir:
- nuevo escenario
- abrir escenario
- guardar escenario
- guardar como
- exportar imagen
- exportar datos
- salir

## 13.2 Menú Ver
Debe incluir:
- mostrar/ocultar panel izquierdo
- mostrar/ocultar inspector
- mostrar/ocultar panel inferior
- activar grilla
- activar capas del escenario

## 13.3 Menú Simulación
Debe incluir:
- iniciar
- pausar
- reiniciar
- elegir método numérico
- configurar paso temporal

## 13.4 Menú Teoría
Debe incluir acceso por categorías de conceptos.

## 13.5 Menú Herramientas
Debe incluir opciones de editor y configuración global.

## 13.6 Menú Ayuda
Debe incluir:
- acerca de
- guía breve
- referencias del proyecto

---

## 14. Toolbar principal

La toolbar debe contener accesos a las operaciones más frecuentes.

### Botones sugeridos
- nuevo
- abrir
- guardar
- teoría
- editor
- simulación
- iniciar
- pausar
- reiniciar
- exportar
- ayuda

Los iconos deben ser sobrios y claros.

---

## 15. Barra de estado

La barra de estado debe ayudar sin distraer.

### Debe poder mostrar
- escenario actual
- coordenadas del cursor sobre el canvas
- modo actual
- método numérico actual
- tiempo actual de simulación
- advertencias breves

---

## 16. Wireframe conceptual de alto nivel

```text
+--------------------------------------------------------------------------------+
| Menu | Toolbar                                                                 |
+--------------------------------------------------------------------------------+
| Panel izquierdo |                 Canvas principal             | Inspector      |
| - navegación    |  escenario, campo, bote, trayectoria        | propiedades    |
| - teoría        |                                              | parámetros     |
| - escenarios    |                                              | motor/campo    |
+--------------------------------------------------------------------------------+
| Pestañas inferiores: Resultados | Gráficas | Eventos | Teoría | Comparación    |
+--------------------------------------------------------------------------------+
| Barra de estado                                                                |
+--------------------------------------------------------------------------------+
```

Este wireframe representa la intención general, no una rigidez absoluta.

---

## 17. Navegación entre modos

La app no debe hacer sentir que son “tres programas distintos”.

### Regla
Teoría, simulación y editor deben convivir dentro de la misma aplicación con navegación clara.

### Formas posibles
- panel de navegación lateral
- pestañas de modo
- botones de modo en toolbar
- combinación de las anteriores

### Recomendación
Usar una navegación principal clara, con transiciones suaves entre vistas sin cerrar la ventana principal.

---

## 18. Uso de diálogos modales

Los diálogos deben usarse solo donde ayuden.

### Buenos usos
- crear escenario nuevo
- renombrar escenario
- exportar imagen o datos
- configurar parámetros globales
- mostrar advertencia crítica

### Malos usos
- forzar demasiados pasos seguidos
- ocultar funciones clave dentro de demasiadas ventanas

---

## 19. Representación visual del escenario

## 19.1 Agua
Debe verse claramente navegable.

## 19.2 Tierra
Debe verse claramente no navegable.

## 19.3 Obstáculos
Deben distinguirse visualmente de tierra general si tienen función diferente.

## 19.4 Campo vectorial
Debe poder representarse con:
- flechas
- densidad ajustable
- colores opcionales por intensidad

## 19.5 Trayectoria
Debe mostrarse como línea clara sobre el escenario.

## 19.6 Bote
Debe mostrar orientación y ser visible incluso con overlays activos.

---

## 20. Representación visual del modo teoría

El modo teoría debe tener una apariencia más contemplativa que el modo simulación.

### Componentes visuales recomendados
- encabezado fuerte
- fórmula grande y clara
- ilustración central pequeña o mediana
- texto corto y legible
- navegación lateral de conceptos
- botón “ver en simulación”

### Sensación
Como objeto de museo digital o ficha técnica interactiva.

---

## 21. Escenarios predeterminados como UX

Los escenarios predeterminados no son solo contenido: también son una pieza de experiencia.

Cada ejemplo debería verse como un experimento listo para abrir.

### Tarjeta o entrada sugerida
- nombre del escenario
- mini descripción
- concepto matemático asociado
- dificultad o complejidad aproximada
- botón abrir

Esto hace que la app sea más amigable desde temprano.

---

## 22. Reglas de claridad visual

1. el canvas central siempre debe dominar
2. los paneles laterales no deben robar protagonismo
3. las fórmulas deben verse limpias, no como texto improvisado
4. la teoría debe ser breve por pantalla
5. los resultados numéricos deben estar agrupados
6. el usuario no debe dudar qué está editando
7. cada modo debe tener una identidad visual reconocible, pero coherente con el resto

---

## 23. Decisiones UX escritas en piedra

1. la aplicación tendrá una **ventana principal única** como centro del trabajo
2. la interfaz será **multipanel**, con canvas central dominante
3. el sistema tendrá **modo teoría**, **modo simulación** y **modo editor**
4. la estética se inspirará en software técnico/educativo de escritorio **2005–2010**
5. la navegación debe sentirse clara, sobria y de laboratorio interactivo
6. el modo teoría tendrá aire de **ficha técnica didáctica tipo Encarta**
7. la app privilegiará **legibilidad y control** por encima de espectacularidad visual gratuita
8. los resultados y gráficas vivirán preferiblemente en paneles inferiores o pestañas separadas

---

## 24. Frase oficial del documento

**La experiencia visual del proyecto debe organizarse como una estación de trabajo de escritorio clásica, con un canvas central para la exploración del fenómeno, paneles de navegación y propiedades, resultados en pestañas y un modo teoría de estilo didáctico, todo orientado a hacer comprensibles y manipulables conceptos matemáticos y físicos.**
