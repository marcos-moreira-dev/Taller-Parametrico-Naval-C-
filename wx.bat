@echo off
setlocal EnableExtensions

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "MINGW_BIN=C:\msys64\mingw64\bin"
set "CMAKE=%MINGW_BIN%\cmake.exe"
set "NINJA=%MINGW_BIN%\ninja.exe"
set "WX_BUILD=%ROOT%\build\wxwidgets"
set "WX_INSTALL=%ROOT%\wx_installed"

for %%I in ("%ROOT%\..\wxWidgets-3.3.2") do set "WX_SOURCE=%%~fI"

if not exist "%MINGW_BIN%" (
    echo [ERROR] No se encontro MinGW en "%MINGW_BIN%".
    exit /b 1
)

if not exist "%WX_SOURCE%\include\wx\wx.h" (
    echo [ERROR] No se encontro el codigo fuente de wxWidgets.
    echo [INFO] Se esperaba en "%WX_SOURCE%".
    exit /b 1
)

set "PATH=%MINGW_BIN%;%PATH%"

echo ============================================
echo   Rebuild wxWidgets
echo ============================================
echo.

if exist "%WX_BUILD%" rmdir /s /q "%WX_BUILD%" >nul 2>&1
if exist "%WX_INSTALL%" rmdir /s /q "%WX_INSTALL%" >nul 2>&1

mkdir "%WX_BUILD%"

echo [INFO] Configurando wxWidgets...
"%CMAKE%" -S "%WX_SOURCE%" -B "%WX_BUILD%" -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=%WX_INSTALL%" -DwxBUILD_SHARED=OFF -DwxUSE_WEBVIEW=ON -DwxUSE_MEDIACTRL=OFF -DwxUSE_STC=OFF -DwxUSE_OPENGL=OFF -DwxUSE_AUI=ON -DwxUSE_HTML=ON
if errorlevel 1 exit /b 1

echo [INFO] Compilando wxWidgets...
"%NINJA%" -C "%WX_BUILD%" -j4
if errorlevel 1 exit /b 1

echo [INFO] Instalando wxWidgets...
"%NINJA%" -C "%WX_BUILD%" install
if errorlevel 1 exit /b 1

echo.
echo [OK] wxWidgets listo en "%WX_INSTALL%".
exit /b 0
