## Idea central

Una ecuación diferencial no dibuja la trayectoria completa de una sola vez. Lo que hace es describir la ley de cambio instantáneo del sistema. La simulación transforma esa ley en movimiento visible integrando paso a paso.

En el bote, la posición cambia según la velocidad y la velocidad cambia según las fuerzas. Esa cadena de dependencias es exactamente lo que expresa un sistema de EDO.

## Ejercicio resuelto

**Problema.** Una lancha de masa $$m=10\,\text{kg}$$ recibe una fuerza neta constante $$\mathbf{F}=(20,0)\,\text{N}$$ y parte del reposo.

**Solución.** La aceleración es

$$
\mathbf{a}=\frac{\mathbf{F}}{m}=(2,0)\,\text{m/s}^2
$$

La velocidad queda

$$
\mathbf{v}(t)=(2t,0)
$$

La posición es

$$
x(t)=t^2
$$

Para $$t=3\,\text{s}$$,

$$
\mathbf{v}(3)=(6,0), \qquad x(3)=9
$$

## Qué observar en la simulación

El método numérico no cambia la física del modelo, pero sí la forma en que esa ley diferencial se aproxima en la computadora.
