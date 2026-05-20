#pragma once

#include <memory>

namespace nova3d::graphics { class IRenderContext; }

namespace nova3d::platform {

class IInput;
class IWindow;

class IPlatformServices {
public:
    virtual ~IPlatformServices() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool pumpEvents() = 0;
    virtual IWindow& window() = 0;
    virtual IInput& input() = 0;
    virtual std::unique_ptr<graphics::IRenderContext> createRenderContext() = 0;
};

} // namespace nova3d::platform
