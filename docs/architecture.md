# Nova3D Architecture

## Platform and Rendering Foundation

Nova3D now uses a strict runtime split between engine-core abstractions and SDL/OpenGL platform implementations.

### Core abstractions

- `IPlatformServices`: SDL lifecycle, event pump, window/input ownership, and render-context creation.
- `IWindow`: window creation, title/fullscreen control, DPI-aware size, and buffer swapping.
- `IInput`: keyboard/mouse/touch/gamepad event polling and quit state.
- `IRenderContext`: context lifecycle, frame begin/end, viewport updates, and clear-color control.
- `IApplication`: sandbox/app hooks (`onInitialize`, `onUpdate`, `onShutdown`).

### Platform separation

All preprocessor platform branching is isolated in the SDL platform implementation and render-context creation.

- Desktop: `SDLWindowDesktop`, `SDLInputDesktop`, `OpenGLRenderContextDesktop`
- Android: `SDLWindowAndroid`, `SDLInputAndroid`, `OpenGLESRenderContextAndroid`

No Win32/X11/NativeActivity API leaks exist in engine core modules.

### Initialization flow

1. `Engine::initialize` creates platform services (`PlatformFactory`).
2. SDL is initialized (`SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER`).
3. SDL window is created with OpenGL/OpenGL ES flags.
4. Render context is created and initialized from platform services.
5. Application callback `onInitialize` is executed.
6. Main loop pumps SDL events, computes frame delta time, clears frame, runs app update, swaps buffers.
7. Shutdown releases app, GL context, SDL window, and SDL subsystems.

### Android strategy

- Android Gradle skeleton is provided under `android/`.
- ABI target is `arm64-v8a`.
- OpenGL ES 3.x is required in `AndroidManifest.xml`.
- Native build is configured through CMake external native build.

This keeps the engine backend-extensible for future Vulkan without changing app-facing abstractions.
