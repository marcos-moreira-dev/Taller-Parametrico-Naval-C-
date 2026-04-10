# PROPULSION_AND_ENERGY — Proyecto de C++: Taller Paramétrico de Navegación y Campos 2D

## 1. Propósito del documento

Este documento define el subdominio de **propulsión y energía** del proyecto.

Su objetivo es dejar claro:

- cómo se modela la fuente energética del bote
- qué tipos de energía o combustible existen en el sistema
- qué unidades se usarán
- cómo se representa el consumo
- cómo se estima la autonomía
- qué simplificaciones físicas y químicas se aceptan
- qué entra al MVP y qué no

Este documento no busca desarrollar una teoría realista de motores marinos ni una química avanzada de combustión. Busca definir un modelo didáctico, coherente y útil para el proyecto.

---

## 2. Motivación del subdominio

El bote no debe modelarse solo como una geometría que se mueve por el escenario. También debe poder representar algo más ingenieril:

- con qué se impulsa
- cuánta energía tiene disponible
- cuánto consume
- si la energía disponible es suficiente
- qué diferencia hay entre distintas fuentes energéticas

Eso permite preguntas didácticas e interesantes como:

- ¿cuánta gasolina necesita este experimento?
- ¿alcanza el combustible para completar la trayectoria?
- ¿qué cambia si el bote usa etanol en vez de gasolina?
- ¿qué pasa si el bote es eléctrico?
- ¿cómo influye la eficiencia del sistema de propulsión?

---

## 3. Filosofía del modelo

El modelo de propulsión y energía debe ser:

- didáctico
- simplificado
- internamente consistente
- ampliable
- entendible para fines de estudio

### Reglas filosóficas

1. no se modelará combustión realista completa
2. no se modelará química profunda del combustible
3. no se modelará termodinámica avanzada del motor
4. sí se modelará energía disponible, consumo y suficiencia
5. sí se permitirá comparar fuentes energéticas bajo hipótesis simplificadas

---

## 4. Alcance conceptual

El subdominio debe cubrir, como mínimo:

- tipo de fuente energética
- capacidad disponible
- potencia o empuje solicitado
- eficiencia simplificada
- consumo acumulado
- energía restante
- autonomía estimada

No debe cubrir, en la primera etapa:

- octanaje
- emisiones reales
- combustión química completa
- degradación real de baterías
- curvas de potencia reales de motores marinos
- temperatura, presión y reacción química detallada

---

## 5. Conceptos centrales

## 5.1 Fuente de energía
Representa el “depósito” o fuente desde la cual el sistema de propulsión obtiene energía útil.

Ejemplos:
- gasolina
- diésel
- etanol
- eléctrico

## 5.2 Sistema de propulsión
Representa el mecanismo que transforma energía disponible en empuje útil.

### Componentes conceptuales
- potencia máxima
- eficiencia
- consumo
- límites operativos simplificados

## 5.3 Consumo
Representa la energía o combustible gastado durante la simulación.

## 5.4 Autonomía
Representa cuánto tiempo o qué distancia puede sostenerse el sistema antes de agotar su energía disponible.

---

## 6. Tipos de energía o combustible a contemplar

El sistema debe contemplar al menos un pequeño catálogo inicial.

## 6.1 Gasolina
Fuente energética líquida.

### Unidad natural de almacenamiento
- litros

### Característica conceptual
- alta energía por unidad de volumen
- combustible clásico fácil de entender didácticamente

## 6.2 Diésel
Fuente energética líquida.

### Unidad natural de almacenamiento
- litros

### Característica conceptual
- alta energía por volumen
- distinto comportamiento energético respecto a gasolina

## 6.3 Etanol
Fuente energética líquida.

### Unidad natural de almacenamiento
- litros

### Característica conceptual
- menor energía volumétrica que gasolina de forma simplificada
- útil para comparación didáctica

## 6.4 Eléctrico
Fuente energética no líquida almacenada en batería.

### Unidad natural de almacenamiento
- kWh

### Característica conceptual
- permite comparar combustibles líquidos con energía almacenada eléctricamente
- útil para autonomía y eficiencia simplificada

---

## 7. Unidades del sistema

Para evitar confusión, el proyecto debe trabajar con un esquema claro de unidades.

## 7.1 Unidades de almacenamiento visibles al usuario

### Para combustibles líquidos
- litro

### Para batería
- kWh

## 7.2 Unidad energética interna recomendada
Para comparación transversal, el sistema debe normalizar la energía a una unidad común interna.

### Recomendación
Usar una unidad energética unificada como:
- MJ (megajulios)
o
- kWh

### Criterio sugerido
Internamente puede ser más natural usar **MJ**, y en UI mostrar:
- litros para combustibles líquidos
- kWh para eléctrico

## 7.3 Potencia
- W o kW

## 7.4 Tiempo
- segundos

## 7.5 Energía consumida
- J, kJ, MJ o kWh según contexto visual

---

## 8. Modelo conceptual de equivalencia energética

Para comparar diferentes combustibles, el proyecto debe definir una propiedad clave:

## 8.1 Energía por unidad de almacenamiento
Cada tipo de fuente energética debe tener una propiedad simplificada del estilo:

- energía por litro
- energía por kWh almacenado

Esto permite calcular:

\[
\text{energía total disponible} = \text{capacidad} \times \text{energía por unidad}
\]

## 8.2 Nota importante
Los valores específicos deben documentarse en un catálogo de datos del proyecto y no quedar dispersos en el código.

---

## 9. Entidades del subdominio

## 9.1 `FuelType` o tipo de fuente energética
Representa una clase de energía o combustible.

### Atributos sugeridos
- `id`
- `name`
- `storage_unit`
- `energy_unit`
- `energy_per_storage_unit`
- `notes`
- `category` (`liquid_fuel`, `electric`, etc.)

## 9.2 `EnergySource`
Representa una instancia concreta de fuente energética para un bote.

### Atributos sugeridos
- `fuel_type`
- `capacity_total`
- `capacity_current`
- `energy_total`
- `energy_remaining`

### Responsabilidades
- calcular energía disponible
- descontar consumo
- indicar si queda suficiente energía

## 9.3 `PropulsionSystem`
Representa el sistema que convierte energía disponible en empuje.

### Atributos sugeridos
- `max_power`
- `efficiency`
- `energy_source`
- `consumption_model`
- `max_thrust` (si aplica)

### Responsabilidades
- convertir solicitud de empuje en demanda energética
- consultar disponibilidad de energía
- registrar consumo

## 9.4 `ConsumptionModel`
Representa la regla de consumo.

### Posibles tipos
- consumo proporcional a potencia instantánea
- consumo base por tiempo
- consumo mixto simplificado

---

## 10. Modelo de consumo recomendado

## 10.1 Modelo base
El proyecto debe usar un modelo sencillo y entendible.

La idea central es:

\[
P(t) = \mathbf{T}(t) \cdot \mathbf{v}(t)
\]

Donde:
- \(P(t)\) es la potencia instantánea útil
- \(\mathbf{T}(t)\) es el empuje
- \(\mathbf{v}(t)\) es la velocidad

La energía consumida se estima mediante:

\[
E = \int_0^T P(t)\,dt
\]

Numéricamente:

\[
E \approx \sum_n P(t_n)\,h
\]

## 10.2 Inclusión de eficiencia
Como el sistema real no es ideal, se puede incluir una eficiencia simplificada:

\[
E_{\text{requerida}} = \frac{E_{\text{útil}}}{\eta}
\]

Donde:
- \(\eta\) es la eficiencia del sistema de propulsión

## 10.3 Conversión a capacidad consumida
Si la fuente energética tiene una energía por unidad de almacenamiento:

\[
\text{capacidad consumida} = \frac{E_{\text{requerida}}}{\text{energía por unidad}}
\]

Eso permite traducir energía a:
- litros consumidos
- kWh consumidos

---

## 11. Modelo mínimo para gasolina, diésel, etanol y eléctrico

## 11.1 Qué se quiere modelar
No interesa modelar la reacción química completa. Solo interesa saber que cada fuente energética:

- almacena energía de manera distinta
- tiene una capacidad definida
- ofrece una cantidad útil de energía por unidad
- puede agotarse

## 11.2 Simplificación práctica
Cada tipo tendrá:

- una unidad visible
- un factor energético
- notas didácticas

Ejemplo conceptual:
- gasolina: más energía por litro que etanol
- diésel: alta energía por litro
- etanol: menor energía por litro que gasolina
- eléctrico: capacidad expresada en kWh

---

## 12. Requerimientos funcionales del subdominio

## 12.1 Selección de fuente energética
El usuario debe poder elegir el tipo de fuente energética del bote.

## 12.2 Definición de capacidad
El usuario debe poder definir la capacidad disponible.

Ejemplos:
- 8 litros de gasolina
- 10 litros de etanol
- 12 kWh de batería

## 12.3 Visualización de estado energético
La aplicación debe mostrar, al menos:
- capacidad inicial
- capacidad restante
- energía estimada consumida
- indicador de suficiencia

## 12.4 Cálculo de suficiencia
La aplicación debe poder responder si la energía disponible es suficiente para completar o sostener un experimento dado.

## 12.5 Comparación entre fuentes
Idealmente, el usuario debe poder repetir el mismo escenario con distintas fuentes energéticas y comparar resultados.

---

## 13. Qué debe verse en la interfaz

El subdominio de energía debe tener presencia clara pero no excesiva.

## 13.1 En el inspector del bote o del motor
Debe poder verse:
- tipo de energía
- capacidad
- eficiencia
- energía restante
- consumo estimado

## 13.2 En resultados
Debe poder verse:
- energía total consumida
- combustible o capacidad gastada
- porcentaje restante
- suficiencia estimada

## 13.3 En modo teoría o ayuda contextual
Puede explicarse:
- diferencia entre potencia y energía
- por qué dos combustibles no equivalen solo por litros
- relación entre eficiencia y autonomía

---

## 14. Relación con física y química

## 14.1 Física involucrada
Este subdominio se apoya principalmente en:
- trabajo
- potencia
- energía
- eficiencia
- relación entre fuerza y movimiento

## 14.2 Química involucrada
La química solo aparece de forma muy ligera como justificación de que diferentes combustibles almacenan distinta energía por unidad.

### Importante
La química no será simulada. Solo se reflejará en parámetros del catálogo energético.

---

## 15. Qué debe ir a un catálogo de datos

Los valores numéricos de referencia no deben quedar escondidos en el código.

## 15.1 Archivo sugerido
- `data/fuels/fuel_catalog.json`
o
- `data/fuels/fuel_catalog.yaml`

## 15.2 Qué debería contener
Por cada tipo de fuente:
- id
- nombre
- unidad de almacenamiento
- unidad energética
- energía por unidad
- notas
- color o icono opcional para la UI

## 15.3 Ventaja
Esto permite:
- cambiar valores sin reescribir lógica
- mantener trazabilidad
- documentar claramente los supuestos del modelo

---

## 16. Escenarios didácticos recomendados para este subdominio

## Escenario A — Mismo trayecto, distinta fuente energética
Comparar gasolina, etanol y eléctrico bajo el mismo recorrido.

## Escenario B — Energía insuficiente
Mostrar cómo el bote no puede sostener cierto experimento por falta de capacidad.

## Escenario C — Cambio de eficiencia
Comparar el mismo combustible con dos eficiencias distintas.

## Escenario D — Motor más exigente
Mostrar cómo un empuje mayor incrementa el gasto energético.

---

## 17. Qué entra al MVP

Para que el alcance no se dispare, el MVP debe incluir solo:

- selección de tipo de fuente energética
- capacidad total definida por el usuario
- eficiencia simplificada
- consumo acumulado
- energía restante
- validación de suficiencia

Eso ya basta para que el subdominio exista con sentido.

---

## 18. Qué queda fuera de la primera etapa

No entra en la primera fase:

- curva real de torque de motor
- combustión detallada
- rendimiento dependiente de revoluciones reales
- degradación de batería
- temperatura del sistema
- peso variable por combustible consumido
- emisiones
- mezcla de combustibles

---

## 19. Riesgos conceptuales a evitar

- tratar litros y kWh como si fueran lo mismo sin una unidad energética común
- meter números arbitrarios sin documentarlos
- esconder el consumo dentro del bote sin modelar la fuente energética
- convertir química ligera en una pseudo-simulación irrealista
- hacer tan complejo el modelo que deje de ser didáctico

---

## 20. Decisiones escritas en piedra

1. el proyecto reconocerá explícitamente un subdominio de **propulsión y energía**
2. el tipo de energía del bote no será un simple texto decorativo
3. las fuentes iniciales contempladas serán, al menos, gasolina, diésel, etanol y eléctrico
4. el sistema usará una **unidad energética común interna** para comparar fuentes
5. el usuario verá la capacidad en su unidad natural (litros o kWh)
6. el consumo se modelará de forma simplificada a partir de potencia, energía y eficiencia
7. la química solo se incorporará de forma ligera como diferencia de energía por unidad entre fuentes
8. los valores energéticos de referencia deberán vivir en un catálogo de datos explícito
9. el MVP priorizará capacidad, consumo, energía restante y suficiencia
10. no se modelará combustión realista ni degradación compleja en la primera etapa

---

## 21. Frase oficial del documento

**El subdominio de propulsión y energía modela de forma didáctica cómo un bote obtiene, transforma y consume energía para generar empuje, permitiendo comparar distintas fuentes energéticas mediante una unidad común interna, una capacidad visible para el usuario y un cálculo simplificado de consumo, eficiencia y autonomía.**
