<p align="center">
  <img src="README_imagenes/logo%20taller%20parametrico.png" alt="Logo oficial de Taller Paramétrico" width="460" />
</p>

<h1 align="center">Taller Paramétrico</h1>
<p align="center"><strong>Navegación y Campos 2D</strong></p>

<p align="center">
  Aplicación educativa de escritorio para estudiar navegación 2D, campos vectoriales,
  métodos numéricos y visualización matemática en un entorno local para Windows.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/version-1.1.0-blue" alt="Version 1.1.0" />
  <img src="https://img.shields.io/badge/C%2B%2B-20-orange" alt="C++20" />
  <img src="https://img.shields.io/badge/platform-Windows%2010%2F11-0078D6" alt="Windows 10/11" />
  <img src="https://img.shields.io/badge/license-GPL--3.0--or--later-blueviolet" alt="GPL 3.0 or later" />
</p>

## Qué es

**Taller Paramétrico** es un software educativo autocontenido pensado para aprender viendo. Combina simulación, teoría guiada y visualización de escenarios para que conceptos como campos vectoriales, Euler, Heun, Runge-Kutta, trayectorias paramétricas, circulación, divergencia o rotor no se queden solo en una fórmula aislada.

La idea del proyecto es sencilla: que el usuario pueda pasar de una explicación matemática a una escena interactiva, modificar parámetros y observar cómo cambia el movimiento del bote dentro del campo. No intenta ser un framework científico generalista. Está enfocado en ser una herramienta clara, didáctica y visual para estudio, práctica y demostración.

## Estado del proyecto

El proyecto ya está en un punto funcional y presentable para GitHub como aplicación de escritorio **Windows-first**.

La política actual del repositorio es esta:

- plataforma oficial: **Windows 10/11 x64**;
- toolchain oficial: **CMake + Ninja + MinGW64 + wxWidgets local**;
- enfoque: **aplicación educativa de escritorio**;
- editor de campo: **experimental / V2**;
- teoría: fortalecida con **figuras estáticas, fórmulas principales y ejercicios guiados**.

## Pantallas del programa

Las imágenes del README viven en la carpeta raíz **`README_imagenes/`**, usando los nombres elegidos para las capturas.

### `pantalla principal.png`

Esta vista resume la identidad general del programa. A la izquierda aparece el panel de navegación con los escenarios disponibles. En el centro se visualiza el escenario activo y a la derecha están las propiedades editables del campo, del bote o de los parámetros numéricos. Es la puerta de entrada al flujo completo del software: elegir un escenario, modificar condiciones y luego simular.

<p align="center">
  <img src="README_imagenes/pantalla%20principal.png" alt="Pantalla principal" width="900" />
</p>

### `teoría.png`

La pantalla de teoría es el corazón didáctico del proyecto. Presenta una organización por conceptos, una descripción con contexto, fórmula principal, figura de estudio, desarrollo explicado y ejercicio guiado. La intención es que el usuario no vea solo “texto bonito”, sino una secuencia pedagógica: idea, expresión matemática, interpretación y aplicación.

<p align="center">
  <img src="README_imagenes/teor%C3%ADa.png" alt="Pantalla de teoría" width="900" />
</p>

### `simulación 3d.png`

Esta captura muestra la escena activa con una presentación más inmersiva del entorno. Aunque el proyecto está centrado en navegación y campos 2D, esta vista sirve para reforzar la percepción espacial del escenario, darle carácter visual al software y volver más atractiva la exploración de configuraciones como el Efecto Venturi o los puertos con obstáculos.

<p align="center">
  <img src="README_imagenes/simulaci%C3%B3n%203d.png" alt="Simulación 3D" width="900" />
</p>

### `simulacion y graficas.png`

Aquí se ve la parte más analítica del programa. La simulación corre mientras se registran gráficas de velocidad, energía y posición. Esta pantalla conecta la experiencia visual con la lectura cuantitativa del método numérico elegido. Es donde el proyecto deja de ser solo una escena interactiva y se convierte en una herramienta de estudio comparativo.

<p align="center">
  <img src="README_imagenes/simulacion%20y%20graficas.png" alt="Simulación y gráficas" width="900" />
</p>

## Qué puedes estudiar dentro del taller

El módulo teórico y la simulación cubren, entre otros temas:

- campos vectoriales;
- ecuaciones diferenciales ordinarias;
- método de Euler;
- método de Heun;
- método de Runge-Kutta de orden 4;
- composición de velocidades y fuerzas;
- campo radial y campo rotacional;
- energía cinética, trabajo y potencia propulsiva;
- trayectorias y curvas paramétricas;
- integral de línea, circulación, divergencia y rotor.

## Flujo de uso

Un recorrido típico dentro del programa es este:

1. elegir un escenario o preset;
2. ajustar parámetros del campo y del bote;
3. ejecutar la simulación;
4. observar trayectoria, vectores y resultados;
5. revisar teoría y ejercicios guiados relacionados con el mismo concepto;
6. comparar el comportamiento visual con las gráficas numéricas.

## Compilación rápida en Windows

```bat
build.bat
```

También puedes usar:

```bat
build.bat run
build.bat rebuild
build.bat test
build.bat release
```

La salida principal queda en:

```text
build\bin\TallerParametrico.exe
```

## Estructura documental

La documentación viva del proyecto está concentrada en:

- `docs/PRODUCT.md`
- `docs/ARCHITECTURE.md`
- `docs/BUILD_WINDOWS.md`
- `docs/THEORY_MODULE.md`
- `docs/ROADMAP.md`
- `docs/INDEX.md`

## Limpieza para GitHub

El repositorio ya está preparado para publicarse con una política más limpia:

- no incluye carpetas de compilación ni artefactos generados;
- `.gitignore` ya excluye ejecutables, logs, caches y builds;
- se eliminó la carpeta antigua de imágenes del README en `docs/` para evitar duplicidad;
- las capturas oficiales del README quedaron centralizadas en `README_imagenes/`.

## Licencia

Este proyecto se distribuye bajo **GPL-3.0-or-later**.

Revisa también:

- `LICENSE`
- `THIRD_PARTY_NOTICES.md`
