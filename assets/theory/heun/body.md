## Idea central

Heun primero predice con Euler y luego corrige. Esa corrección usa la pendiente final del paso y la promedia con la inicial. El resultado es una aproximación mucho más fiel con un coste todavía moderado.

Puede verse como una forma sencilla del método trapezoidal: en lugar de una sola pendiente, utiliza una pendiente promedio.

## Ejercicio resuelto

**Problema.** Resolver

$$
\frac{dy}{dt}=y,
\qquad
y(0)=1,
\qquad
h=0.1
$$

Se calcula primero

$$
k_1 = 1, \qquad y^* = 1 + 0.1\cdot 1 = 1.1
$$

Luego

$$
k_2 = 1.1
$$

Finalmente

$$
y_1 = 1 + \frac{0.1}{2}(1+1.1)=1.105
$$

La exacta vale

$$
e^{0.1}\approx 1.10517
$$

## Qué observar en la simulación

Heun suele capturar mejor la forma global de la trayectoria cuando el flujo cambia moderadamente dentro del paso temporal.
