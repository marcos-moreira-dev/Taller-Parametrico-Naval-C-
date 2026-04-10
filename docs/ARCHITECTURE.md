# Arquitectura

## Capas principales

- `src/domain`: entidades y lógica base del dominio.
- `src/simulation`: integradores y física de simulación.
- `src/application`: coordinación de casos de uso y documentos de escenario.
- `src/education`: catálogo, repositorio y render básico del módulo teórico.
- `src/presentation`: ventana principal, paneles y widgets wxWidgets.
- `src/shared`: utilidades transversales, logging, constantes y tipos.

## Observaciones de refactor

La mayor deuda actual vive en `src/presentation`, pero ya se redujo una parte importante: `MainWindow` fue dividido por responsabilidades y `LeftPanel`, `RightPanel` y `BottomPanel` se movieron a archivos propios. La política de esta etapa sigue siendo refactor serio, no reescritura completa.

## Decisiones vigentes

- Se privilegia mantenibilidad sobre portabilidad forzada.
- La teoría puede usar imágenes estáticas generadas externamente si eso mejora claridad.
- La simulación dinámica permanece dentro del cliente C++.


## Estructura actual de presentación

- `src/presentation/main_window/MainWindow.cpp`: bootstrap visual, layout, barra de estado y actualización general.
- `src/presentation/main_window/MainWindowDocument.cpp`: archivo, exportación, dialogs y acciones de documento.
- `src/presentation/main_window/MainWindowView.cpp`: cambios de modo y visibilidad.
- `src/presentation/main_window/MainWindowSimulation.cpp`: ciclo de simulación y eventos numéricos.
- `src/presentation/panels/*`: paneles laterales/inferiores ya extraídos del shell principal.
