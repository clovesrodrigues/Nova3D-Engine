#pragma once

namespace nova3d::core { class NovaDevice; }

namespace nova3d::core {

class IApplication {
public:
    virtual ~IApplication() = default;
    virtual bool onInitialize(NovaDevice& device) = 0;
    virtual void onUpdate(float deltaTimeSeconds) = 0;
    virtual void onShutdown() = 0;
};

} // namespace nova3d::core
