@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "BUILD_DIR=%ROOT%\build"
set "WX_DIR=%ROOT%\build\wx_installed"
set "MODE_FILE=%BUILD_DIR%\.build-mode"
set "TARGET=TallerParametrico"
set "BUILD_GUI=ON"
set "BUILD_CONSOLE=OFF"
set "BUILD_TESTS=OFF"
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
if /I "%~1"=="test" (
    set "ACTION=test"
    set "BUILD_GUI=OFF"
    set "BUILD_CONSOLE=ON"
    set "BUILD_TESTS=ON"
    goto :args_done
)
if /I "%~1"=="release" (
    set "ACTION=release"
    goto :args_done
)
if /I "%~1"=="msi" (
    set "ACTION=msi"
    goto :args_done
)
if /I "%~1"=="wx" (
    set "ACTION=wx"
    goto :args_done
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
if "%ACTION%"=="test" goto :test
if "%ACTION%"=="release" goto :release
if "%ACTION%"=="msi" goto :msi
if "%ACTION%"=="wx" goto :rebuild_wx

if "%MODE%"=="gui" (
    if not exist "%WX_DIR%\include\wx\wx.h" (
        echo [INFO] wxWidgets no esta instalado en `build\wx_installed`.
        echo [INFO] Intentando instalarlo automaticamente...
        call :rebuild_wx
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
exit /b 0

:clean
echo [INFO] Limpiando build del proyecto...
call :clean_build_tree
if exist "%MODE_FILE%" del /q "%MODE_FILE%" >nul 2>&1
echo [OK] Limpieza completada.
exit /b 0

:test
set "TEST_BUILD_DIR=%BUILD_DIR%\tests"
echo [INFO] Configurando tests...
"%CMAKE%" -S "%ROOT%" -B "%TEST_BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=OFF -DBUILD_CONSOLE=ON -DBUILD_TESTS=ON
if errorlevel 1 goto :fail
echo [INFO] Compilando tests...
"%NINJA%" -C "%TEST_BUILD_DIR%" test_boat test_boat_extended test_motor test_scenario test_vector_field test_integrators test_integrators_convergence test_physics test_experiment_service test_simulation_coordinator test_preset_scenarios
if errorlevel 1 goto :fail
echo [INFO] Ejecutando suite de tests...
pushd "%TEST_BUILD_DIR%" >nul
ctest --output-on-failure
if errorlevel 1 (
    popd >nul
    goto :fail
)
popd >nul
echo [OK] Todos los tests pasaron.
exit /b 0

:release
call :prepare_release
if errorlevel 1 goto :fail
echo [OK] Release preparado en "%BUILD_DIR%\release".
exit /b 0

:prepare_release
set "RELEASE_DIR=%BUILD_DIR%\release"
echo [INFO] Preparando release...
call :clean_build_tree
if exist "%MODE_FILE%" del /q "%MODE_FILE%" >nul 2>&1
if exist "%RELEASE_DIR%" rmdir /s /q "%RELEASE_DIR%" >nul 2>&1
mkdir "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%\bin"
mkdir "%RELEASE_DIR%\assets"
mkdir "%RELEASE_DIR%\docs"
"%CMAKE%" -S "%ROOT%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_GUI=ON -DBUILD_CONSOLE=OFF -DBUILD_TESTS=OFF
if errorlevel 1 goto :fail
"%NINJA%" -C "%BUILD_DIR%" TallerParametrico
if errorlevel 1 goto :fail
copy "%BUILD_DIR%\bin\TallerParametrico.exe" "%RELEASE_DIR%\bin\" >nul
copy "%BUILD_DIR%\bin\*.dll" "%RELEASE_DIR%\bin\" >nul 2>&1
xcopy /e /i /y "%ROOT%\assets" "%RELEASE_DIR%\assets\" >nul
xcopy /e /i /y "%ROOT%\docs" "%RELEASE_DIR%\docs\" >nul
copy "%ROOT%\README.md" "%RELEASE_DIR%\" >nul
exit /b 0

:msi
set "WIX_BIN=%WIX%\bin"
if "%WIX%"=="" (
    echo [ERROR] La variable WIX no esta definida.
    echo [INFO] Instala WiX Toolset y define WIX.
    exit /b 1
)
if not exist "%WIX_BIN%\candle.exe" (
    echo [ERROR] No se encontro candle.exe en "%WIX_BIN%".
    exit /b 1
)
call :prepare_release
if errorlevel 1 goto :fail
if not exist "%ROOT%\installer\obj" mkdir "%ROOT%\installer\obj"
if not exist "%ROOT%\installer\bin" mkdir "%ROOT%\installer\bin"
del /q "%ROOT%\installer\obj\*.*" >nul 2>&1
echo [INFO] Compilando instalador WiX...
pushd "%ROOT%\installer" >nul
"%WIX_BIN%\candle.exe" -out obj\ TallerParametrico.wxs
if errorlevel 1 (
    popd >nul
    goto :fail
)
"%WIX_BIN%\light.exe" -out bin\TallerParametrico.msi -ext WixUIExtension -ext WixUtilExtension -cultures:es-ES obj\TallerParametrico.wixobj
if errorlevel 1 (
    popd >nul
    goto :fail
)
popd >nul
echo [OK] MSI creado en "%ROOT%\installer\bin\TallerParametrico.msi".
exit /b 0

:rebuild_wx
set "WX_BUILD=%ROOT%\build\wxwidgets"
set "WX_INSTALL=%ROOT%\build\wx_installed"
for %%I in ("%ROOT%\..\wxWidgets-3.3.2") do set "WX_SOURCE=%%~fI"
if not exist "%WX_SOURCE%\include\wx\wx.h" (
    echo [ERROR] No se encontro el codigo fuente de wxWidgets en "%WX_SOURCE%".
    exit /b 1
)
echo [INFO] Recompilando wxWidgets...
if exist "%WX_BUILD%" rmdir /s /q "%WX_BUILD%" >nul 2>&1
if exist "%WX_INSTALL%" rmdir /s /q "%WX_INSTALL%" >nul 2>&1
mkdir "%WX_BUILD%"
"%CMAKE%" -S "%WX_SOURCE%" -B "%WX_BUILD%" -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=%WX_INSTALL%" -DwxBUILD_SHARED=OFF -DwxUSE_WEBVIEW=ON -DwxUSE_MEDIACTRL=OFF -DwxUSE_STC=OFF -DwxUSE_OPENGL=OFF -DwxUSE_AUI=ON -DwxUSE_HTML=ON
if errorlevel 1 goto :fail
"%NINJA%" -C "%WX_BUILD%" -j4
if errorlevel 1 goto :fail
"%NINJA%" -C "%WX_BUILD%" install
if errorlevel 1 goto :fail
echo [OK] wxWidgets listo en "%WX_INSTALL%".
exit /b 0

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
echo   build.bat test       ^(compila y ejecuta todos los tests^)
echo   build.bat release    ^(prepara build_release/^)
echo   build.bat msi        ^(genera instalador MSI con WiX^)
echo   build.bat wx         ^(recompila wxWidgets local^)
exit /b 0

:end
exit /b 0
