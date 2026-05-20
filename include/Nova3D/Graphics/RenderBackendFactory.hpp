#pragma once
#include <memory>
#include <Nova3D/Graphics/IRenderBackend.hpp>

namespace nova3d::graphics {
std::unique_ptr<IRenderBackend> createRenderBackend();
}
