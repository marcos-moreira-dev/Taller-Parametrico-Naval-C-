@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "BUILD_DIR=%ROOT%\build"
set "WX_DIR=%ROOT%\wx_installed"
set "MODE_FILE=%BUILD_DIR%\.build-mode"
set "TARGET=TallerParametrico"
set "BUILD_GUI=ON"
set "BUILD_CONSOLE=OFF"
set "ACTION=build"
set "MODE=gui"
set "RUN_AFTER=0"
set "NEED_RECONFIGURE=0"

set "MINGW_BIN=C:\msys64\mingw64\bin"
set "CMAKE=%MINGW_BIN%\cmake.exe"
set "NINJA=%MINGW_BIN%\ninja.exe"

if not exist "%MINGW_BIN%" goto :missing_mingw
set "PATH=%MINGW_BIN%;%PATH%"

if /I "%~1"=="" goto :args_done
if /I "%~1"=="gui" goto :args_done
if /I "%~1"=="run" (
    set "RUN_AFTER=1"
    goto :args_done
)
if /I "%~1"=="rebuild" (
    set "ACTION=rebuild"
    goto :args_done
)
if /I "%~1"=="clean" (
    set "ACTION=clean"
    goto :args_done
)
if /I "%~1"=="console" (
    set "MODE=console"
    set "TARGET=TallerParametrico_Console"
    set "BUILD_GUI=OFF"
    set "BUILD_CONSOLE=ON"
    goto :args_done
)
if /I "%~1"=="wx" (
    call "%ROOT%\wx.bat"
    exit /b %errorlevel%
)
if /I "%~1"=="help" goto :help
if /I "%~1"=="-h" goto :help
if /I "%~1"=="--help" goto :help

echo [ERROR] Opcion no valida: %~1
echo.
goto :help

:args_done
echo ============================================
echo   Taller Parametrico - Build
echo ============================================
echo.

if "%ACTION%"=="clean" goto :clean

if "%MODE%"=="gui" (
    if not exist "%WX_DIR%\include\wx\wx.h" (
        echo [INFO] wxWidgets no esta instalado en `wx_installed`.
        echo [INFO] Intentando instalarlo automaticamente...
        call "%ROOT%\wx.bat"
        if errorlevel 1 exit /b 1
    )
)

if "%ACTION%"=="rebuild" (
    echo [INFO] Rebuild solicitado. Limpiando configuracion...
    call :clean_build_tree
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

if not exist "%BUILD_DIR%\build.ninja" set "NEED_RECONFIGURE=1"
if not exist "%BUILD_DIR%\CMakeCache.txt" set "NEED_RECONFIGURE=1"
if not exist "%MODE_FILE%" set "NEED_RECONFIGURE=1"

if exist "%MODE_FILE%" (
    set /p LAST_MODE=<"%MODE_FILE%"
    if /I not "!LAST_MODE!"=="%MODE%" set "NEED_RECONFIGURE=1"
)

if "%NEED_RECONFIGURE%"=="0" (
    "%NINJA%" -C "%BUILD_DIR%" -t targets | findstr /C:"%TARGET%" >nul
    if errorlevel 1 set "NEED_RECONFIGURE=1"
)

if "%NEED_RECONFIGURE%"=="1" goto :configure
goto :build

:configure
echo [INFO] Configurando CMake para modo %MODE%...
"%CMAKE%" -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=%BUILD_GUI% -DBUILD_CONSOLE=%BUILD_CONSOLE% -DBUILD_TESTS=OFF
if errorlevel 1 goto :fail
> "%MODE_FILE%" echo %MODE%

:build
echo [INFO] Compilando %TARGET%...
"%NINJA%" -C "%BUILD_DIR%" %TARGET%
if errorlevel 1 goto :fail

echo.
echo [OK] Build exitoso.
echo [OK] Ejecutable: "%BUILD_DIR%\bin\%TARGET%.exe"

if "%RUN_AFTER%"=="1" goto :run
goto :end

:run
if not exist "%BUILD_DIR%\bin\%TARGET%.exe" (
    echo [ERROR] No se encontro el ejecutable.
    exit /b 1
)
echo [INFO] Ejecutando %TARGET%.exe...
start "" "%BUILD_DIR%\bin\%TARGET%.exe"
goto :end

:clean
echo [INFO] Limpiando build del proyecto...
call :clean_build_tree
if exist "%MODE_FILE%" del /q "%MODE_FILE%" >nul 2>&1
echo [OK] Limpieza completada.
goto :end

:clean_build_tree
if exist "%BUILD_DIR%\bin" rmdir /s /q "%BUILD_DIR%\bin" >nul 2>&1
if exist "%BUILD_DIR%\CMakeFiles" rmdir /s /q "%BUILD_DIR%\CMakeFiles" >nul 2>&1
if exist "%BUILD_DIR%\CMakeCache.txt" del /q "%BUILD_DIR%\CMakeCache.txt" >nul 2>&1
if exist "%BUILD_DIR%\build.ninja" del /q "%BUILD_DIR%\build.ninja" >nul 2>&1
if exist "%BUILD_DIR%\.ninja_deps" del /q "%BUILD_DIR%\.ninja_deps" >nul 2>&1
if exist "%BUILD_DIR%\.ninja_log" del /q "%BUILD_DIR%\.ninja_log" >nul 2>&1
exit /b 0

:missing_mingw
echo [ERROR] No se encontro MinGW en "%MINGW_BIN%".
echo [INFO] Instala MSYS2/MinGW64 o ajusta la ruta en `build.bat`.
exit /b 1

:fail
echo.
echo [ERROR] El proceso fallo.
exit /b 1

:help
echo Uso:
echo   build.bat            ^(GUI inteligente^)
echo   build.bat run        ^(compila GUI y ejecuta^)
echo   build.bat rebuild    ^(reconfigura GUI y recompila^)
echo   build.bat clean      ^(limpia build del proyecto^)
echo   build.bat console    ^(compila version consola^)
echo   build.bat wx         ^(recompila wxWidgets^)
exit /b 0

:end
exit /b 0
