#pragma once

#include <memory>
#include <Nova3D/Platform/IInput.hpp>
#include <Nova3D/Platform/IPlatformServices.hpp>
#include <Nova3D/Platform/IWindow.hpp>

namespace nova3d::platform {

class SDLWindowDesktop final : public IWindow {
public:
    bool create(const WindowDesc& desc) override;
    void destroy() override;
    void setTitle(const std::string& title) override;
    void setFullscreen(bool enabled) override;
    void getSize(int& width, int& height) const override;
    float getDPIScale() const override;
    void swapBuffers() override;
    void* nativeHandle() const override;
    SDL_Window* sdlWindow() const override;
private:
    SDL_Window* m_window = nullptr;
};

class SDLWindowAndroid final : public SDLWindowDesktop {};

class SDLInputDesktop final : public IInput {
public:
    void processEvent(const void* sdlEvent) override;
    void update() override;
    bool isQuitRequested() const override;
    bool isKeyDown(int scancode) const override;
private:
    bool m_quitRequested = false;
};

class SDLInputAndroid final : public SDLInputDesktop {};

class SDLPlatformServices final : public IPlatformServices {
public:
    bool initialize() override;
    void shutdown() override;
    bool pumpEvents() override;
    IWindow& window() override;
    IInput& input() override;
    std::unique_ptr<graphics::IRenderContext> createRenderContext() override;
private:
    std::unique_ptr<IWindow> m_window;
    std::unique_ptr<IInput> m_input;
};

} // namespace nova3d::platform
