# Guía de Inicio Rápido

## Windows

### 1. Compilar el Proyecto

Abre PowerShell en la carpeta del proyecto y ejecuta:

```powershell
# Compilar todo
.\deploy.bat -a

# O paso a paso:
.\deploy.bat -c    # Limpiar y compilar
.\deploy.bat -t    # Ejecutar tests
.\deploy.bat -r    # Ejecutar aplicación
```

### 2. Verificar la Instalación

Si la compilación es exitosa, verás:

```
[SUCCESS] Proceso de despliegue completado!
```

Los ejecutables estarán en: `build\dev-debug\bin\`

### 3. Ejecutar la Versión Consola

```powershell
.\deploy.bat -r
```

Esto ejecutará `TallerParametrico_Console.exe` que mostrará:

- Información del sistema
- Resultados de pruebas básicas
- Resultados de la simulación

### 4. Solución de Problemas

**Si falla la compilación:**

```powershell
# Ver el error detallado
.\deploy.bat -v

# O manualmente:
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
```

**Si falta wxWidgets:**
El proyecto automáticamente compila la versión de consola sin GUI.

## Linux / macOS

### 1. Compilar el Proyecto

```bash
chmod +x deploy.sh
./deploy.sh -a
```

### 2. Ejecutar

```bash
./deploy.sh -r
```

## Estructura de Archivos Generados

```
build/dev-debug/
├── bin/
│   ├── TallerParametrico_Console.exe  <- Versión consola
│   └── TallerParametrico.exe          <- Versión GUI (si hay wxWidgets)
├── lib/
│   ├── tp_domain.lib
│   ├── tp_simulation.lib
│   └── ...
└── tests/
    ├── test_domain.exe
    └── test_simulation.exe
```

## Logs

Todos los logs se guardan en la carpeta `logs/`:

- `logs/deploy_*.log` - Logs del proceso de compilación
- `logs/application.log` - Logs de la aplicación en ejecución
- `logs/console_test.log` - Logs de las pruebas en consola

## Próximos Pasos

Una vez compilado:

1. **Explora el código** en `src/` - está organizado por módulos
2. **Ejecuta los tests** con `./deploy.bat -t`
3. **Modifica los escenarios** en `data/scenarios/`
4. **Añade nuevos campos vectoriales** en `src/domain/VectorField.cpp`
5. **Experimenta con diferentes métodos numéricos**

## Ayuda Adicional

- Ver opciones del script: `deploy.bat --help`
- Ver README principal: `README.md`
- Ver documentación: `docs/`
