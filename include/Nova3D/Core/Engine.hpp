#pragma once

#include <memory>

namespace nova3d::core {
class IApplication;
}
namespace nova3d::platform { class IPlatformServices; }
namespace nova3d::graphics { class IRenderContext; }

namespace nova3d::core {

class Engine {
public:
    bool initialize(IApplication& app);
    void run();
    void shutdown();
private:
    IApplication* m_app = nullptr;
    std::unique_ptr<platform::IPlatformServices> m_platform;
    std::unique_ptr<graphics::IRenderContext> m_renderContext;
    bool m_running = false;
};

} // namespace nova3d::core
