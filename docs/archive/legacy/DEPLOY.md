# Despliegue y Release

## Build local

### Windows

```powershell
.\build.bat
.\build.bat test
```

### Linux/macOS

```bash
./scripts/build.sh
./scripts/run_all_tests.sh
```

## Preparar release

### Windows

```powershell
.\build.bat release
```

Esto genera una carpeta `build_release/` con:

- ejecutable
- DLLs necesarias
- assets
- documentación

## Crear instalador MSI

Requiere WiX Toolset instalado.

```powershell
.\build.bat msi
```

Salida esperada:

```text
installer/bin/TallerParametrico.msi
```

## Verificación previa a release

1. Compilar en modo release
2. Ejecutar toda la suite de tests
3. Verificar que `build/bin/TallerParametrico.exe` abre correctamente
4. Generar `build_release/`
5. Construir MSI

## Estructura de release

```text
build_release/
├── bin/
├── assets/
├── docs/
└── README.md
```
