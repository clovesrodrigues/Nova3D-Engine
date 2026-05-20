#pragma once

namespace nova3d::platform {

class IPlatformServices {
public:
    virtual ~IPlatformServices() = default;
    virtual bool initialize() = 0;
    virtual bool pumpEvents() = 0;
};

} // namespace nova3d::platform
