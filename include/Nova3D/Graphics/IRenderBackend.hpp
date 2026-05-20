#pragma once

namespace nova3d::graphics {

class IRenderBackend {
public:
    virtual ~IRenderBackend() = default;
    virtual bool initialize() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
};

} // namespace nova3d::graphics
