## Idea central

Un campo vectorial es una regla local: a cada punto del plano le asigna un vector. En la simulación eso significa que el bote no siente “todo el río” al mismo tiempo, sino la corriente exactamente en la zona donde está.

Si el campo cambia de una región a otra, la trayectoria cambia aunque el empuje del motor no se modifique. Esta es la razón por la que una embarcación puede acelerar, frenar o desviar su rumbo al entrar en una franja de agua distinta.

## Lectura mecánica

- La **dirección** del vector indica hacia dónde empuja el fluido.
- La **magnitud** indica cuán intensa es la corriente.
- La variación espacial del campo explica por qué el movimiento puede deformarse al cambiar de zona.

## Ejercicio resuelto

**Problema.** Una lancha está en $$(x,y)=(10,25)$$ dentro de un río uniforme con $$\mathbf{V}=(2,0)\,\text{m/s}$$. El piloto genera una velocidad propia relativa al agua de $$\mathbf{v}_p=(3,1)\,\text{m/s}$$.

**Solución.** La velocidad respecto del suelo es

$$
\mathbf{v}_{\text{total}} = \mathbf{v}_p + \mathbf{V} = (3,1) + (2,0) = (5,1)
$$

La rapidez vale

$$
\lVert\mathbf{v}_{\text{total}}\rVert = \sqrt{5^2+1^2} = \sqrt{26} \approx 5.10\,\text{m/s}
$$

El ángulo de deriva es

$$
\theta = \operatorname{atan2}(1,5) \approx 11.3^\circ
$$

## Qué observar en la simulación

Cambia el tipo de campo y observa cómo la orientación del bote deja de coincidir con la dirección real de avance.
