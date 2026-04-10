# Guía de Inicio Rápido

## Windows

### Compilar

Desde la raíz del proyecto:

```powershell
.\build.bat
```

Opciones útiles:

```powershell
.\build.bat run       # Compila y ejecuta
.\build.bat rebuild   # Reconfigura y recompila
.\build.bat clean     # Limpia el build
.\build.bat console   # Compila versión consola
.\build.bat wx        # Recompila wxWidgets local
```

### Ejecutar tests

```powershell
.\build.bat test
```

### Ejecutar aplicación

```powershell
.\build\bin\TallerParametrico.exe
```

## Linux / macOS

### Compilar

```bash
./scripts/build.sh
```

### Ejecutar tests

```bash
./scripts/run_all_tests.sh
```

## Estructura actual de salida

```text
build/
├── bin/
│   ├── TallerParametrico.exe
│   └── TallerParametrico_Console.exe
└── tests/
```

## Recursos del proyecto

- Escenarios y presets: `assets/presets/`
- Contenido teórico: `assets/theory/`
- Código fuente: `src/`
- Tests: `tests/`

## Ayuda adicional

- README principal: `README.md`
- Arquitectura: `docs/ARCHITECTURE_CONSOLIDATED.md`
- Deploy/release: `docs/DEPLOY.md`
