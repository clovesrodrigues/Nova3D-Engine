@echo off
setlocal
set PRESET=%1
if "%PRESET%"=="" set PRESET=mingw-headless

echo [Nova3D] Configurando validator com preset %PRESET%...
cmake --preset %PRESET%
if errorlevel 1 (
  echo [ERRO] Configure falhou.
  exit /b 1
)

echo [Nova3D] Compilando validator...
cmake --build --preset %PRESET% --target nova_asset_validator
if errorlevel 1 (
  echo [ERRO] Build do validator falhou.
  exit /b 1
)

echo [OK] Validator compilado.
endlocal
