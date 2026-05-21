#pragma once

#include "NIEventReceiver.hpp"

#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace N {

struct AInputEvent;

class NCInputManager {
public:
    static constexpr std::size_t NMAX_KEY_CODES = 256;

    NCInputManager() = default;

    void nAddReceiver(NIEventReceiver* nReceiver);
    void nRemoveReceiver(const NIEventReceiver* nReceiver);

    bool nDispatchEvent(const NSEvent& nEvent);
    bool nProcessAndroidInput(AInputEvent* nAndroidEvent);

    bool nIsKeyDown(NKEY_CODE nKey) const;
    bool nWasKeyPressedThisFrame(NKEY_CODE nKey) const;
    void nBeginFrame();

    std::array<std::int32_t, 2> nGetMousePosition() const;
    std::unordered_map<std::int32_t, NSTouchInput> nGetActiveTouches() const;

private:
    static std::size_t nKeyToIndex(NKEY_CODE nKey) noexcept;
    void nApplyEventToState(const NSEvent& nEvent);

    mutable std::mutex nMutex;
    std::bitset<NMAX_KEY_CODES> nKeysDown;
    std::bitset<NMAX_KEY_CODES> nKeysPressedThisFrame;
    std::array<std::int32_t, 2> nMousePosition{0, 0};
    std::unordered_map<std::int32_t, NSTouchInput> nActiveTouches;
    std::vector<NIEventReceiver*> nReceivers;
};

} // namespace N
