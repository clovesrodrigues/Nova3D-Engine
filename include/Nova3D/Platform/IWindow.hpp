#pragma once

#include <string>

struct SDL_Window;

namespace nova3d::platform {

struct WindowDesc {
    int width = 1280;
    int height = 720;
    std::string title = "Nova3D Sandbox";
    bool fullscreen = false;
    bool highDPI = true;
    bool resizable = true;
};

class IWindow {
public:
    virtual ~IWindow() = default;
    virtual bool create(const WindowDesc& desc) = 0;
    virtual void destroy() = 0;
    virtual void setTitle(const std::string& title) = 0;
    virtual void setFullscreen(bool enabled) = 0;
    virtual void getSize(int& width, int& height) const = 0;
    virtual float getDPIScale() const = 0;
    virtual void swapBuffers() = 0;
    virtual void* nativeHandle() const = 0;
    virtual SDL_Window* sdlWindow() const = 0;
};

} // namespace nova3d::platform
