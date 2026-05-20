#include "SDLPlatformServices.hpp"
#include <Nova3D/Core/Logger.hpp>
#include <SDL.h>
#include <Nova3D/Graphics/IRenderContext.hpp>
#include "../../Graphics/OpenGL/OpenGLRenderBackend.hpp"

namespace nova3d::platform {

bool SDLWindowDesktop::create(const WindowDesc& desc) {
    const auto flags = static_cast<Uint32>(SDL_WINDOW_OPENGL |
        (desc.resizable ? SDL_WINDOW_RESIZABLE : 0) |
        (desc.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) |
        (desc.highDPI ? SDL_WINDOW_ALLOW_HIGHDPI : 0));

    m_window = SDL_CreateWindow(desc.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, desc.width, desc.height, flags);
    return m_window != nullptr;
}

void SDLWindowDesktop::destroy() { if (m_window) SDL_DestroyWindow(m_window); m_window = nullptr; }
void SDLWindowDesktop::setTitle(const std::string& title) { SDL_SetWindowTitle(m_window, title.c_str()); }
void SDLWindowDesktop::setFullscreen(bool enabled) { SDL_SetWindowFullscreen(m_window, enabled ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0); }
void SDLWindowDesktop::getSize(int& width, int& height) const { SDL_GL_GetDrawableSize(m_window, &width, &height); }
float SDLWindowDesktop::getDPIScale() const {
    int w = 0, h = 0, dw = 0, dh = 0;
    SDL_GetWindowSize(m_window, &w, &h);
    SDL_GL_GetDrawableSize(m_window, &dw, &dh);
    return w > 0 ? static_cast<float>(dw) / static_cast<float>(w) : 1.0F;
}
void SDLWindowDesktop::swapBuffers() { SDL_GL_SwapWindow(m_window); }
void* SDLWindowDesktop::nativeHandle() const { return m_window; }
SDL_Window* SDLWindowDesktop::sdlWindow() const { return m_window; }

void SDLInputDesktop::processEvent(const void* sdlEventPtr) {
    const auto& event = *static_cast<const SDL_Event*>(sdlEventPtr);
    if (event.type == SDL_QUIT) m_quitRequested = true;
}
void SDLInputDesktop::update() {}
bool SDLInputDesktop::isQuitRequested() const { return m_quitRequested; }
bool SDLInputDesktop::isKeyDown(int scancode) const {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    return state && state[scancode] != 0;
}

bool SDLPlatformServices::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) {
        core::Logger::error(std::string("SDL_Init failed: ") + SDL_GetError());
        return false;
    }
#if defined(__ANDROID__)
    m_window = std::make_unique<SDLWindowAndroid>();
    m_input = std::make_unique<SDLInputAndroid>();
#else
    m_window = std::make_unique<SDLWindowDesktop>();
    m_input = std::make_unique<SDLInputDesktop>();
#endif
    WindowDesc desc;
    if (!m_window->create(desc)) {
        core::Logger::error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        return false;
    }
    core::Logger::info("SDL platform initialized");
    return true;
}

void SDLPlatformServices::shutdown() {
    if (m_window) m_window->destroy();
    SDL_Quit();
}

bool SDLPlatformServices::pumpEvents() {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        m_input->processEvent(&event);
    }
    m_input->update();
    return !m_input->isQuitRequested();
}

IWindow& SDLPlatformServices::window() { return *m_window; }
IInput& SDLPlatformServices::input() { return *m_input; }
std::unique_ptr<graphics::IRenderContext> SDLPlatformServices::createRenderContext() {
#if defined(__ANDROID__)
    return std::make_unique<graphics::OpenGLESRenderContextAndroid>();
#else
    return std::make_unique<graphics::OpenGLRenderContextDesktop>();
#endif
}

} // namespace nova3d::platform
