#include <Nova3D/Core/Engine.hpp>
#include <Nova3D/Core/IApplication.hpp>
#include <Nova3D/Core/Logger.hpp>
#include <Nova3D/Platform/PlatformFactory.hpp>
#include <chrono>

namespace nova3d::core {

bool Engine::initialize(IApplication& app) {
    m_app = &app;
    m_platform = platform::createPlatformServices();
    if (!m_platform || !m_platform->initialize()) {
        Logger::error("Platform init failed");
        return false;
    }

    m_renderContext = m_platform->createRenderContext();
    if (!m_renderContext || !m_renderContext->initialize(m_platform->window())) {
        Logger::error("Render context init failed");
        return false;
    }

    if (!m_app->onInitialize()) {
        Logger::error("Application init failed");
        return false;
    }

    Logger::info("Nova3D initialized");
    m_running = true;
    return true;
}

void Engine::run() {
    using clock = std::chrono::steady_clock;
    auto previous = clock::now();
    while (m_running && m_platform->pumpEvents()) {
        const auto now = clock::now();
        const float delta = std::chrono::duration<float>(now - previous).count();
        previous = now;
        m_renderContext->beginFrame();
        m_app->onUpdate(delta);
        m_renderContext->endFrame();
    }
    shutdown();
}

void Engine::shutdown() {
    if (m_app) m_app->onShutdown();
    if (m_renderContext) m_renderContext->shutdown();
    if (m_platform) m_platform->shutdown();
    m_running = false;
    Logger::info("Nova3D shutdown complete");
}

} // namespace nova3d::core
