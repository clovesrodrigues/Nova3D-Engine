#pragma once

#include <Nova3D/Platform/IPlatformServices.hpp>
#include <Nova3D/Platform/IInput.hpp>
#include <Nova3D/Platform/IWindow.hpp>
#include <Nova3D/Core/Engine.hpp>

namespace nova::platform {

class NInputState {
public:
    explicit NInputState(nova3d::platform::IInput& input) : m_input(input) {}
    bool isKeyDown(int scancode) const { return m_input.isKeyDown(scancode); }
private:
    nova3d::platform::IInput& m_input;
};

class NWindow {
public:
    explicit NWindow(nova3d::platform::IWindow& window) : m_window(window) {}
    void getSize(int& width, int& height) const { m_window.getSize(width, height); }
    void setTitle(const std::string& title) { m_window.setTitle(title); }
private:
    nova3d::platform::IWindow& m_window;
};

class NEventPump {
public:
    explicit NEventPump(nova3d::platform::IPlatformServices& platform) : m_platform(platform) {}
    bool poll() { return m_platform.pumpEvents(); }
private:
    nova3d::platform::IPlatformServices& m_platform;
};

class NApplication {
public:
    virtual ~NApplication() = default;
    virtual bool onInitialize(nova3d::core::NovaDevice& device) = 0;
    virtual void onUpdate(float dt, NInputState& input) = 0;
    virtual void onShutdown() {}
};

} // namespace nova::platform
