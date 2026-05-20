#include <Nova3D/Core/Engine.hpp>
#include <Nova3D/Core/Logger.hpp>
#include <Nova3D/Platform/PlatformFactory.hpp>
#include <Nova3D/Graphics/RenderBackendFactory.hpp>

namespace nova3d::core {

bool Engine::initialize() {
    auto platform = platform::createPlatformServices();
    if (!platform->initialize()) {
        Logger::error("Platform init failed");
        return false;
    }

    auto renderer = graphics::createRenderBackend();
    if (!renderer->initialize()) {
        Logger::error("Renderer init failed");
        return false;
    }

    Logger::info("Nova3D initialized");
    return true;
}

void Engine::run() {
    Logger::info("Engine run loop placeholder");
}

} // namespace nova3d::core
