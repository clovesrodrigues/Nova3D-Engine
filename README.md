# Nova3D Engine

Nova3D é uma engine 3D multiplataforma inspirada na filosofia das melhores Engines, construída com C++ moderno e arquitetura modular/desacoplada.

## Objetivos

- Núcleo desacoplado de plataforma, render, assets, input, áudio, GUI, cena, física e animação.
- API unificada com interfaces + fábricas para escolha automática por alvo de compilação.
- Build híbrido Desktop/Mobile com SDL2 + OpenGL/OpenGL ES.
- Base preparada para expansão futura (Vulkan, ferramentas externas wxWidgets).

## Estrutura

```text
.
├── CMakeLists.txt
├── Makefile
├── cmake/
│   └── Toolchains/
│       └── android-ndk.cmake
├── include/Nova3D/
│   ├── Core/
│   ├── Platform/
│   ├── Graphics/
│   ├── Assets/
│   ├── Scene/
│   ├── Physics/
│   ├── Animation/
│   ├── Audio/
│   └── GUI/
├── src/
│   ├── Core/
│   ├── Platform/
│   ├── Graphics/
│   ├── Assets/
│   ├── Scene/
│   ├── Physics/
│   ├── Animation/
│   ├── Audio/
│   └── GUI/
├── apps/
│   └── Sandbox/
└── docs/
    ├── architecture.md
    └── roadmap.md
```

## Requisitos

- CMake 3.24+
- Ninja
- SDL2
- OpenGL (desktop)
- OpenGL ES 3.0+ (Android)
- Android NDK r25+

## Build Desktop

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/apps/Sandbox/nova_sandbox
```

## Build Android (NDK)

```bash
cmake -S . -B build-android -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=cmake/Toolchains/android-ndk.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=24
cmake --build build-android
```

## Build alternativo via Makefile

```bash
make configure
make build
make run
```

## Princípios de design

- `#if defined(ANDROID)` **somente** na camada de plataforma.
- Núcleo da engine nunca usa APIs específicas de janela/OS diretamente.
- Subsystems expostos por interfaces e compostos por factories.
- Backends de render desacoplados da lógica da cena.
- Importers/extensões de assets baseados em registro dinâmico.

## Licença

MIT

## S2 hardening: módulos e validação

Novos módulos públicos:
- `nova::platform`: `NWindow`, `NEventPump`, `NInputState`, `NApplication`.
- `nova::asset`: `NModelImporter` + backend Assimp com estatísticas de import.
- `nova::audio`: `NAudioDevice`, `NAudioClip`, `NAudioVoiceHandle`.

Dependências de runtime (Windows):
- `bin/SDL2.dll`
- `bin/libassimp-6.dll`
- `lib/*.a` (incluindo SDL2/GLFW/SoLoud/Assimp import libs)

> `lib/` e `bin/` fazem parte do pacote atual do projeto e permanecem versionados neste repositório.

Assets de teste **não são obrigatórios** no repositório principal.
Forneça localmente seus arquivos (ex.: `assets/models/seu_modelo.obj`, `assets/models/seu_modelo.fbx`, `assets/audio/seu_audio.wav`) para validação/sandbox.

### Configuração resiliente

Quando OpenGL de desenvolvimento não estiver disponível em desktop:
- padrão: falha explícita com mensagem de instalação.
- fallback controlado: `-DNOVA3D_REQUIRE_OPENGL=OFF` para configurar targets não-gráficos.

### Como validar localmente

```bash
cmake -S . -B build -DNOVA3D_REQUIRE_OPENGL=OFF
cmake --build build -j
./build/nova_asset_validator
```

Com OpenGL presente, também rode o sandbox:

```bash
cmake -S . -B build
cmake --build build -j
./build/apps/Sandbox/nova_sandbox
```

## Compilar em 1 minuto

### Pré-requisitos
- Windows + MinGW (`g++`, `mingw32-make` no PATH)
- CMake 3.24+
- OpenGL de desenvolvimento (para sandbox gráfico)

### Via scripts `.bat`
```bat
build_engine.bat mingw-release
build_sandbox.bat mingw-debug
build_validator.bat mingw-headless
run_sandbox.bat mingw-debug
```

### Via CMake presets
```bat
cmake --preset mingw-headless
cmake --build --preset mingw-headless --target nova_asset_validator

cmake --preset mingw-debug
cmake --build --preset mingw-debug --target nova_sandbox
```

### Troubleshooting rápido
- Erro de OpenGL no configure: usar preset `mingw-headless` (sem sandbox gráfico).
- DLL ausente ao rodar sandbox: execute `build_sandbox.bat` novamente para copiar DLLs.
- Falha de toolchain MinGW: confirme `g++ --version` e `mingw32-make --version` no terminal.
