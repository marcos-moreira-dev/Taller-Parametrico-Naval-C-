# Build en Windows

## Requisitos

- MSYS2 / MinGW64
- `cmake.exe`
- `ninja.exe`
- wxWidgets compilado localmente o instalado en `build\wx_installed`

## Script oficial

```bat
build.bat
```

## Comandos útiles

```bat
build.bat run
build.bat rebuild
build.bat test
build.bat release
```

## Salidas esperadas

- Ejecutable GUI: `buildin\TallerParametrico.exe`
- Release empaquetado: `buildelease\`
- Logs de aplicación: `logspplication.log`
