# Instalador MSI para Taller Paramétrico

Este directorio contiene los archivos necesarios para crear un instalador MSI profesional usando WiX Toolset.

## Requisitos

- WiX Toolset v3.11 o superior (https://wixtoolset.org/)
- Visual Studio 2019/2022 con componente "WiX Toolset Visual Studio Extension"
- O usar línea de comandos con `candle.exe` y `light.exe`

## Estructura

```
installer/
├── TallerParametrico.wxs      # Definición principal del instalador
├── Variables.wxi              # Variables configurables
├── Features.wxs               # Características del producto
├── UI.wxs                     # Interfaz de usuario personalizada
├── LICENSE.rtf                # Licencia en formato RTF
├── banner.bmp                 # Banner del instalador (493x58)
├── dialog.bmp                 # Imagen lateral (493x312)
```

## Uso

### Método 1: Desde la raíz del proyecto (recomendado)

```bash
build.bat msi
```

### Método 2: Línea de comandos manual

```bash
# Compilar
"%WIX%\bin\candle.exe" -out obj\ TallerParametrico.wxs

# Enlazar
"%WIX%\bin\light.exe" -out bin\TallerParametrico.msi obj\TallerParametrico.wixobj
```

### Método 3: Visual Studio

1. Abrir `TallerParametrico.sln`
2. Cambiar configuración a "Release"
3. Build → Build Solution
4. El MSI se genera en `installer/bin/`

## Personalización

Editar `Variables.wxi` para cambiar:
- Versión del producto
- Nombre del fabricante
- Directorio de instalación por defecto
- GUIDs del producto (generar nuevos para cada release mayor)

## Características del Instalador

- ✅ Instalación silenciosa disponible (`msiexec /i TallerParametrico.msi /quiet`)
- ✅ Soporte para instalación por usuario o por máquina
- ✅ Verificación de requisitos previos (VC++ Redistributable)
- ✅ Creación de accesos directos en menú inicio y escritorio
- ✅ Integración con "Agregar o quitar programas"
- ✅ Soporte para desinstalación limpia
- ✅ Localización en español

## Firma Digital (Opcional)

Para firmar el instalador:

```bash
signtool.exe sign /f certificate.pfx /p password /t http://timestamp.digicert.com bin\TallerParametrico.msi
```
