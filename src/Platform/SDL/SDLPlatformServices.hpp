#pragma once
#include <Nova3D/Platform/IPlatformServices.hpp>

namespace nova3d::platform {

class SDLPlatformServices final : public IPlatformServices {
public:
    bool initialize() override;
    bool pumpEvents() override;
};

} // namespace nova3d::platform
