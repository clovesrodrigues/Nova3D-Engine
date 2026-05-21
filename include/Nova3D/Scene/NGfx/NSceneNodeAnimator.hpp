#pragma once

#include <cstdint>

namespace NEngine {

class NISceneNode;

/**
 * @brief Base RAII animator behavior for scene graph nodes.
 *
 * Lifecycle ownership is exclusive to a scene node through std::unique_ptr,
 * allowing deterministic teardown and zero manual memory management.
 */
class NISceneNodeAnimator {
public:
    virtual ~NISceneNodeAnimator() = default;
    virtual void nAnimateNode(NISceneNode* nNode, uint32_t nTimeMs) = 0;

    [[nodiscard]] bool nIsFinished() const noexcept { return nHasFinished; }

protected:
    bool nHasFinished{false};
};

} // namespace NEngine
