# Módulo teórico

## Política visual

Cada tema debe tender a esta estructura:

1. descripción corta con contexto;
2. fórmula principal limpia;
3. figura de estudio estática cuando convenga;
4. visualización del concepto;
5. desarrollo y lectura conceptual;
6. ejercicio guiado moderado.

## Assets por tema

Cada carpeta en `assets/theory/<tema>/` puede contener:

- `meta.json`
- `body.md`
- `formula.tex`
- `exercise.md`
- `figure_main.png`

## Criterios

- Las fórmulas multilinea deben escribirse con alineación explícita cuando sea necesario.
- El contenido debe sentirse como ficha de estudio, no como nota suelta.
- Las imágenes estáticas pueden generarse con Python y quedar versionadas dentro del repo.
