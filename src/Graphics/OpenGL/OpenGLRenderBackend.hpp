#pragma once

#include <Nova3D/Graphics/IRenderContext.hpp>

using SDL_GLContext = void*;

namespace nova3d::graphics {

class OpenGLRenderContextDesktop final : public IRenderContext {
public:
    bool initialize(platform::IWindow& window) override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void resize(int width, int height) override;
    void setClearColor(float r, float g, float b, float a) override;
private:
    platform::IWindow* m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    float m_clearColor[4] = {0.1F, 0.15F, 0.2F, 1.0F};
};

class OpenGLESRenderContextAndroid final : public OpenGLRenderContextDesktop {};

} // namespace nova3d::graphics
