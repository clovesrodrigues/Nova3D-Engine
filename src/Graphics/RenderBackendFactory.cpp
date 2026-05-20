#include <Nova3D/Graphics/RenderBackendFactory.hpp>
#include "OpenGL/OpenGLRenderBackend.hpp"

namespace nova3d::graphics {

std::unique_ptr<IRenderBackend> createRenderBackend() {
    return std::make_unique<OpenGLRenderBackend>();
}

} // namespace nova3d::graphics
