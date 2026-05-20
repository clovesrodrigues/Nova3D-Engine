#pragma once

namespace nova3d::platform { class IWindow; }
namespace nova3d::graphics { class IVideoDriver; }

namespace nova3d::graphics {

class IRenderContext {
public:
    virtual ~IRenderContext() = default;
    virtual bool initialize(platform::IWindow& window) = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void resize(int width, int height) = 0;
    virtual IVideoDriver& videoDriver() = 0;
};

} // namespace nova3d::graphics
