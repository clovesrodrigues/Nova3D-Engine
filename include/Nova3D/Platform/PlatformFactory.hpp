#pragma once
#include <memory>
#include <Nova3D/Platform/IPlatformServices.hpp>

namespace nova3d::platform {
std::unique_ptr<IPlatformServices> createPlatformServices();
}
