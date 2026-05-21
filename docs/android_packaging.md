# Android Packaging (produto separado)

## Estrutura proposta (inicial)

```text
sdk/
├── include/                         # headers comuns Nova3D
└── lib/
    └── android/
        └── arm64-v8a/
            ├── libnova3d.so
            ├── libassimp.so         # quando aplicável ao pacote Android
            └── outras dependências .so por ABI
```

## O que não misturar com desktop
- Não incluir `*.dll` de Windows no pacote Android.
- Não incluir `*.dll.a` (import libs MinGW/MSYS) no pacote Android.
- Não compartilhar diretórios `bin/` desktop como fonte de runtime Android.

## Fluxo mínimo de consumo (CMake no app Android)

```cmake
add_library(nova3d SHARED IMPORTED)
set_target_properties(nova3d PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/sdk/lib/android/arm64-v8a/libnova3d.so"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/sdk/include"
)

target_link_libraries(your_native_lib PRIVATE nova3d)
```

## Fluxo mínimo de consumo (Gradle)
- Usar `externalNativeBuild.cmake` para o módulo nativo.
- Manter `abiFilters "arm64-v8a"` na fase inicial.
- Incluir `.so` em `src/main/jniLibs/arm64-v8a/` **ou** resolver via caminho explícito no CMake do consumidor.

Exemplo resumido (`build.gradle.kts`):

```kotlin
android {
  defaultConfig {
    ndk { abiFilters += listOf("arm64-v8a") }
    externalNativeBuild { cmake { cppFlags += "-std=c++20" } }
  }
  externalNativeBuild { cmake { path = file("src/main/cpp/CMakeLists.txt") } }
}
```

## Checklist de ABI
- [ ] ABI inicial definida: `arm64-v8a`.
- [ ] Todas as `.so` de terceiros existem para `arm64-v8a`.
- [ ] `include/` comum alinhado com versão da `libnova3d.so`.
- [ ] Estratégia registrada para expansão futura (`armeabi-v7a`, `x86_64`) sem quebrar layout.
