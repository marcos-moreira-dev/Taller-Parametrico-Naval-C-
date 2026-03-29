## Idea central

Euler toma la pendiente actual y supone que esa tendencia se mantiene durante un intervalo corto $$h$$. Con esa hipótesis, avanza un paso en el tiempo.

Es el método numérico más simple y por eso es perfecto para aprender. También es el más fácil de implementar. Su limitación es que la recta tangente local no siempre representa bien la curvatura real de la solución.

## Ejercicio resuelto

**Problema.** Resolver

$$
\frac{dv}{dt}=2,
\qquad
v(0)=1,
\qquad
h=0.5\,\text{s}
$$

**Paso 1**

$$
v_1 = v_0 + h\cdot 2 = 1 + 0.5\cdot 2 = 2
$$

**Paso 2**

$$
v_2 = v_1 + h\cdot 2 = 2 + 0.5\cdot 2 = 3
$$

En este caso Euler coincide con la solución exacta porque la derivada es constante. En trayectorias curvadas esa coincidencia desaparece.

## Qué observar en la simulación

Compara Euler con RK4 en un remolino: verás que el error de trayectoria crece mucho más rápido.
