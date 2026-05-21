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


## Build & SDK Targets

Documentação operacional oficial:

- Matriz de build e compatibilidade: `docs/build_matrix.md`
- Empacotamento Android separado: `docs/android_packaging.md`
- Política de suporte por formato: `docs/format_support_policy.md`
- Checklist de release estável de SDK: `docs/sdk_release_checklist.md`

## Princípios de design

- `#if defined(ANDROID)` **somente** na camada de plataforma.
- Núcleo da engine nunca usa APIs específicas de janela/OS diretamente.
- Subsystems expostos por interfaces e compostos por factories.
- Backends de render desacoplados da lógica da cena.
- Importers/extensões de assets baseados em registro dinâmico.

## Licença

MIT

## Import regression tests

Run the import regression suite:

```bash
cmake --build build --target nova_asset_validator
./build/apps/AssetValidator/nova_asset_validator assets/tests/import
```

Format policy matrix: `docs/import_format_matrix.md`.

## GUI Advanced Usage

- Create/assign skin:
  - `auto skin = std::make_shared<nova3d::gui::NGUISkin>();`
  - `nova3d::gui::GUIThemeManager themeMgr(skin);`
  - `themeMgr.setDarkTheme();`, `themeMgr.setLightTheme();`, `themeMgr.setCustomAccent({r,g,b,a});`
- Register widgets in `NGUIElementFactory` and load JSON layout:
  - `NGUILayoutLoader::loadLayoutJson("assets/gui/sandbox_layout.json", ctx, factory, skin);`
- Show dialogs via unified API:
  - `NFileDialog::open/save/selectFolder`
  - `NColorDialog::pick`
  - `NMessageBox::show`
- Use mesh viewer widget:
  - `NMeshViewerWidget` supports wheel zoom, mouse drag orbit, `R` reset, `W` wireframe toggle.


## Sandbox Scene Demo Controls (Fase 4)
- Camera mode toggles between FPS and Orbit in the runtime demo sequence.
- FPS controls are fed via animator input channels (`setMoveInput`, `setLookInput`) and collision response is active.
- Orbit controls use target orbit + dolly.
- Picking runs from screen center each frame and logs hit distance/triangle metrics.
- Scene includes collidable mesh nodes and an active billboard node foundation.

Format support notes:
- **Official**: 3DS, B3D, IRR/IRRMESH, LWO/LWS, MD2, X.
- **Experimental**: Blender native `.blend` (use FBX/OBJ/GLTF intermediary in current pipeline).
