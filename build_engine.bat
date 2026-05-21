@echo off
setlocal
set PRESET=%1
if "%PRESET%"=="" set PRESET=mingw-release

echo [Nova3D] Configurando preset %PRESET%...
cmake --preset %PRESET%
if errorlevel 1 (
  echo [ERRO] Falha no configure. Verifique dependencias e preset.
  exit /b 1
)

echo [Nova3D] Compilando biblioteca principal...
cmake --build --preset %PRESET% --target nova3d
if errorlevel 1 (
  echo [ERRO] Falha ao compilar nova3d.
  exit /b 1
)

echo [OK] Engine compilada com sucesso.
endlocal
