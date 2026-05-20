#include "OpenGLRenderBackend.hpp"
#include <Nova3D/Core/Logger.hpp>
#include <Nova3D/Platform/IWindow.hpp>
#include <SDL.h>
#if defined(__ANDROID__)
#include <GLES3/gl3.h>
#else
#include <SDL_opengl.h>
#endif

namespace nova3d::graphics {

bool OpenGLRenderContextDesktop::initialize(platform::IWindow& window) {
    m_window = &window;
#if defined(__ANDROID__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_context = SDL_GL_CreateContext(window.sdlWindow());
    if (!m_context) {
        core::Logger::error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
        return false;
    }
    SDL_GL_SetSwapInterval(1);
    int w = 0;
    int h = 0;
    window.getSize(w, h);
    resize(w, h);
    return true;
}

void OpenGLRenderContextDesktop::shutdown() {
    if (m_context) {
        SDL_GL_DeleteContext(m_context);
        m_context = nullptr;
    }
}

void OpenGLRenderContextDesktop::beginFrame() {
    glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderContextDesktop::endFrame() {
    if (m_window) {
        m_window->swapBuffers();
    }
}

void OpenGLRenderContextDesktop::resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void OpenGLRenderContextDesktop::setClearColor(float r, float g, float b, float a) {
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

} // namespace nova3d::graphics
