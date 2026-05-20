#include <Nova3D/Platform/PlatformFactory.hpp>
#include "SDL/SDLPlatformServices.hpp"

namespace nova3d::platform {

std::unique_ptr<IPlatformServices> createPlatformServices() {
#if defined(ANDROID)
    return std::make_unique<SDLPlatformServices>();
#else
    return std::make_unique<SDLPlatformServices>();
#endif
}

} // namespace nova3d::platform
