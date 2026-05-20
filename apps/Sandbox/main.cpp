#include <Nova3D/Core/Engine.hpp>
#include <Nova3D/Core/IApplication.hpp>

class SandboxApp final : public nova3d::core::IApplication {
public:
    bool onInitialize() override { return true; }
    void onUpdate(float) override {}
    void onShutdown() override {}
};

int main() {
    SandboxApp app;
    nova3d::core::Engine engine;
    if (!engine.initialize(app)) {
        return 1;
    }
    engine.run();
    return 0;
}
