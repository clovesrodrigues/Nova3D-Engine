@echo off
setlocal
set PRESET=%1
if "%PRESET%"=="" set PRESET=mingw-debug

echo [Nova3D] Configurando sandbox com preset %PRESET%...
cmake --preset %PRESET%
if errorlevel 1 (
  echo [ERRO] Configure falhou. Se faltar OpenGL use build_validator.bat mingw-headless.
  exit /b 1
)

echo [Nova3D] Compilando sandbox...
cmake --build --preset %PRESET% --target nova_sandbox
if errorlevel 1 (
  echo [ERRO] Build do sandbox falhou.
  exit /b 1
)

echo [Nova3D] Copiando DLLs de runtime...
if exist bin\SDL2.dll copy /Y bin\SDL2.dll build\%PRESET%\apps\Sandbox\ >nul
if exist bin\libassimp-6.dll copy /Y bin\libassimp-6.dll build\%PRESET%\apps\Sandbox\ >nul
if exist bin\soloud_x64.dll copy /Y bin\soloud_x64.dll build\%PRESET%\apps\Sandbox\ >nul

echo [OK] Sandbox compilado.
endlocal
