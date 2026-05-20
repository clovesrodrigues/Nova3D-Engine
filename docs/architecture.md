# Arquitetura Nova3D

## Camadas

- **Core**: memória, containers, matemática, filesystem, XML, threading, logs.
- **Platform**: janela/input/timers/filesystem por plataforma, via SDL2.
- **Graphics**: materiais, shaders, backend, texturas, iluminação, pós-processamento, scene graph.
- **Assets**: registry de importadores para meshes, texturas, shaders, áudio e pacotes ZIP/PAK/PK3.
- **Scene/Physics/Animation/Audio/GUI**: módulos independentes com contrato por interface.

## Diretrizes principais

1. `#if defined(ANDROID)` fica isolado em `src/Platform`.
2. `Core` e lógica de alto nível não conhecem Win32/X11/JNI.
3. Fábricas realizam seleção de implementações (`createPlatformServices`, `createRenderBackend`).
4. Componentes são versionáveis e testáveis de forma independente.

## Render API unificada

`IRenderBackend` expõe operações mínimas de frame para suportar OpenGL desktop e OpenGL ES Android sob mesmo contrato.

## Extensibilidade planejada

- Backends futuros: Vulkan/Metal/Direct3D como novos módulos.
- Ferramentas externas futuras (wxWidgets) usando as mesmas APIs públicas de assets/cena.
- Importadores adicionais sem alterar o core (registro por extensão).
