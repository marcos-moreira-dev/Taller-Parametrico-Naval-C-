# Script de Despliegue - Taller Parametrico

## Uso

### Windows (PowerShell)

```powershell
# Solo compilar
.\deploy.bat

# Limpiar y compilar
.\deploy.bat -c

# Compilar y ejecutar tests
.\deploy.bat -t

# Compilar y ejecutar la aplicación
.\deploy.bat -r

# Todo: limpiar, compilar, testear y ejecutar
.\deploy.bat -a

# Modo Release
.\deploy.bat --release
```

### Linux/macOS (Bash)

```bash
# Solo compilar
./deploy.sh

# Limpiar y compilar
./deploy.sh -c

# Compilar y ejecutar tests
./deploy.sh -t

# Compilar y ejecutar la aplicación
./deploy.sh -r

# Todo: limpiar, compilar, testear y ejecutar
./deploy.sh -a

# Modo Release
./deploy.sh --release
```

## Opciones

- `-h, --help`: Muestra la ayuda
- `-c, --clean`: Limpia el build antes de compilar
- `-t, --test`: Ejecuta tests después de compilar
- `-r, --run`: Ejecuta la aplicación después de compilar
- `-a, --all`: Limpia, compila, testea y ejecuta (todo)
- `--release`: Usa configuración Release (por defecto: Debug)

## Logs

Los logs de compilación y ejecución se guardan en:

- `logs/deploy_YYYYMMDD_HHMMSS.log`
- `logs/application.log` (logs de la aplicación)

## Estructura de Salida

```
build/
  dev-debug/
    bin/
      TallerParametrico.exe  (Windows)
      TallerParametrico      (Linux/macOS)
    lib/
      tp_domain.lib
      tp_simulation.lib
      ...
    tests/
      test_domain.exe
      test_simulation.exe
      ...
```
