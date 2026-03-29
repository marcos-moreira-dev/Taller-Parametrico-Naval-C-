## Idea central

RK4 consulta varias veces la pendiente dentro del mismo paso temporal y combina esas evaluaciones con pesos distintos. Con ello reconstruye mejor la curvatura local de la solución.

Su coste por paso es mayor, pero la ganancia en precisión suele compensarlo cuando el movimiento tiene giro, arrastre o cambios rápidos de campo.

## Ejercicio resuelto

**Problema.** Resolver

$$
\frac{dy}{dt}=-2y,
\qquad
y(0)=1,
\qquad
h=0.2
$$

Las pendientes intermedias son

$$
k_1=-2,
\qquad
k_2=-1.6,
\qquad
k_3=-1.68,
\qquad
k_4=-1.328
$$

Entonces

$$
y_1 = 1 + \frac{0.2}{6}\left(-2-3.2-3.36-1.328\right)=0.6704
$$

La solución exacta es

$$
y_{\text{exacta}}(0.2)=e^{-0.4}\approx 0.6703
$$

## Qué observar en la simulación

En trayectorias largas o remolinos marcados, RK4 preserva mejor la forma del movimiento y reduce el error acumulado.
