# Guía de wxWidgets

## Windows

El proyecto usa un flujo local para wxWidgets:

1. `build.bat` intenta usar `build/wx_installed/` si existe.
2. Si no existe, `build.bat wx` recompila wxWidgets localmente.
3. La fuente esperada está en:

```text
..\wxWidgets-3.3.2
```

### Recompilar wxWidgets

```powershell
.\build.bat wx
```

Esto genera dentro de `build/`:

- `build/wxwidgets/` (árbol de compilación)
- `build/wx_installed/` (instalación local reutilizable)

## Linux / macOS

Se recomienda usar la instalación del sistema:

### Ubuntu/Debian

```bash
sudo apt-get install libwxgtk3.2-dev
```

### macOS

```bash
brew install wxwidgets
```

## Si CMake no encuentra wxWidgets

Verifica:

1. que existe una instalación del sistema, o
2. que `..\wxWidgets-3.3.2` existe en Windows antes de ejecutar `build.bat wx`

## Notas

- `build/wx_installed/` no debe versionarse en Git
- `wxwidgets/` tampoco debe versionarse
- ambos directorios se regeneran cuando haga falta
