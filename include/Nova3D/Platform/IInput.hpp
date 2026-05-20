#pragma once

namespace nova3d::platform {

class IInput {
public:
    virtual ~IInput() = default;
    virtual void processEvent(const void* sdlEvent) = 0;
    virtual void update() = 0;
    virtual bool isQuitRequested() const = 0;
    virtual bool isKeyDown(int scancode) const = 0;
};

} // namespace nova3d::platform
