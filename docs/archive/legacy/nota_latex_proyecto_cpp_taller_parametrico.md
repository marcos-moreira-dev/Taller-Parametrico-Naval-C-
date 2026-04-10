# Nota — Soporte LaTeX en el proyecto de C++

## Propósito

Esta nota deja explícito que la aplicación debe contemplar soporte para mostrar fórmulas matemáticas en formato LaTeX o equivalente renderizado legible en pantalla.

## Dónde debe aparecer

El soporte de fórmulas debe considerarse, al menos, en estos contextos:

- modo teoría
- funciones del campo definidas por el usuario
- funciones del motor o empuje
- resultados analíticos cuando existan
- resúmenes matemáticos de un experimento
- comparaciones entre solución analítica y aproximación numérica

## Regla funcional

La aplicación no necesita ser un editor completo de LaTeX, pero sí debe poder:

- mostrar fórmulas de manera clara
- asociarlas a conceptos del modo teoría
- mostrar expresiones matemáticas relevantes del experimento actual

## Alcance del MVP

Para la primera versión, basta con:

- renderizado de fórmulas predefinidas del modo teoría
- visualización legible de expresiones matemáticas clave del sistema

## Alcance futuro

En etapas posteriores, podría permitirse:

- mayor cantidad de expresiones generadas dinámicamente
- mejor presentación tipográfica
- integración más rica con funciones definidas por el usuario

## Decisión escrita en piedra

El proyecto debe contemplar soporte de visualización matemática tipo LaTeX como parte de su identidad didáctica y técnica.