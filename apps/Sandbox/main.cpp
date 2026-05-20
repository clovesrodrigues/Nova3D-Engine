#include <Nova3D/Core/Engine.hpp>

int main() {
    nova3d::core::Engine engine;
    if (!engine.initialize()) {
        return 1;
    }
    engine.run();
    return 0;
}
