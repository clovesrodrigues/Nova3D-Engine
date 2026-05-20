#include "SDLPlatformServices.hpp"
#include <SDL.h>

namespace nova3d::platform {

bool SDLPlatformServices::initialize() {
    return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) == 0;
}

bool SDLPlatformServices::pumpEvents() {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }
    return true;
}

} // namespace nova3d::platform
