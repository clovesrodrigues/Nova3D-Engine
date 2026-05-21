# Nova3D Build Matrix Oficial

## Matriz por target

| Target | Compilador | Gerador CMake recomendado | Runtime esperado | Artefatos de saída | Conjunto de libs em `lib/` e `bin/` | Status atual | Riscos / pendências técnicas |
|---|---|---|---|---|---|---|---|
| Windows x64 (MinGW64) | `gcc/g++` (MinGW-w64) | Ninja | `nova3d.dll` + import libs (`.dll.a`) e DLLs de runtime | `build/windows-mingw-*/` com `nova3d` + apps | `lib/*.a`, `lib/*.dll.a`, `bin/*.dll` (SDL2, Assimp, SoLoud, GLEW) | **Ready** | Garantir sincronia de versões das DLLs redistribuídas com headers e ABI do build.
| Linux x64 | `gcc/g++` (ou clang/clang++) | Ninja | `libnova3d.a`/`libnova3d.so` + libs ELF dependentes | `build/linux-gcc-debug/` com engine e utilitários | Reaproveita headers em `include/`; empacotamento de `lib/` Linux ainda pendente | **Planned** | Padronizar pacote de dependências Linux (SDL2/OpenGL/Assimp/SoLoud) e política de RPATH.
| Android arm64-v8a | NDK `clang/clang++` | Ninja via toolchain Android | `libnova3d.so` + dependências `.so` por ABI | `build/android-arm64-debug/` e saída de packaging Android | Produto separado em `lib/android/arm64-v8a/*.so` (não misturar com desktop) | **Planned** | Requer `ANDROID_NDK` configurado e pacote dedicado de terceiros por ABI.

## Regras de compatibilidade

### Compatível entre targets
- API C++ pública em `include/Nova3D` (tipos/headers de contrato).
- Contrato de importação via tipos Nova3D (`NModelData`, `NImportReport` etc.), independente do backend.
- Convenções de CMake (`NOVA3D_BUILD_SANDBOX`, presets oficiais) para fluxo de build repetível.

### Específico por plataforma
- **Windows Desktop:** distribuição de DLLs em `bin/` e import libs em `lib/`.
- **Linux Desktop:** artefatos ELF e resolução de dependências via linker do sistema.
- **Android:** distribuição por ABI em `.so`, com integração via `externalNativeBuild`/`jniLibs`; sem uso de DLL/import-lib de Windows.
