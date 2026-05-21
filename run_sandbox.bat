@echo off
setlocal
set PRESET=%1
if "%PRESET%"=="" set PRESET=mingw-debug
set BIN=build\%PRESET%\apps\Sandbox\nova_sandbox.exe
if not exist "%BIN%" (
  echo [INFO] Sandbox nao encontrado. Executando build_sandbox.bat...
  call build_sandbox.bat %PRESET%
  if errorlevel 1 exit /b 1
)

echo [Nova3D] Executando sandbox...
"%BIN%"
endlocal
