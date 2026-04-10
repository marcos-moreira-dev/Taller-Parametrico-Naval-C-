# Changelog de refactor

## 1.2.0 - Refactor estructural y limpieza documental

- `MainWindow` se dividió en varios archivos por responsabilidad.
- `LeftPanel`, `RightPanel` y `BottomPanel` se extrajeron a archivos propios.
- Se introdujeron `MainWindowIds.hpp` y `MainWindowStyles.hpp` para reducir duplicación.
- Se añadieron trazas de acciones clave en modos, archivos y simulación.
- Se archivó la documentación heredada en `docs/archive/legacy/` y se dejó un índice limpio en `docs/INDEX.md`.
- Se quitó `EquationRenderer.cpp` del build activo al no formar parte del flujo oficial actual.
