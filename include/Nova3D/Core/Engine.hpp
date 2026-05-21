#pragma once

#include <memory>
#include <Nova3D/Graphics/IRenderContext.hpp>

namespace nova3d::core { class IApplication; }
namespace nova3d::platform { class IPlatformServices; }
namespace nova3d::graphics { class IVideoDriver; }

namespace nova3d::core {

class NovaDevice {
public:
    bool initialize(IApplication& app);
    void run();
    void shutdown();

    platform::IPlatformServices& platformServices();
    graphics::IRenderContext& renderContext();
    graphics::IVideoDriver& videoDriver();
private:
    IApplication* m_app = nullptr;
    std::unique_ptr<platform::IPlatformServices> m_platform;
    std::unique_ptr<graphics::IRenderContext> m_renderContext;
    bool m_running = false;
};

using Engine = NovaDevice;

} // namespace nova3d::core
