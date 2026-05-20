#pragma once
#include <Nova3D/Graphics/IRenderBackend.hpp>

namespace nova3d::graphics {

class OpenGLRenderBackend final : public IRenderBackend {
public:
    bool initialize() override;
    void beginFrame() override;
    void endFrame() override;
};

} // namespace nova3d::graphics
