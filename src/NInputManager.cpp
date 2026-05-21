#include "NInputManager.hpp"

#include <algorithm>
#include <limits>
#include <utility>

#ifdef __ANDROID__
#include <android/input.h>
#endif

namespace N {

void NCInputManager::nAddReceiver(NIEventReceiver* nReceiver) {
    if (nReceiver == nullptr) {
        return;
    }

    std::scoped_lock nLock(nMutex);
    if (std::find(nReceivers.begin(), nReceivers.end(), nReceiver) == nReceivers.end()) {
        nReceivers.push_back(nReceiver);
    }
}

void NCInputManager::nRemoveReceiver(const NIEventReceiver* nReceiver) {
    std::scoped_lock nLock(nMutex);
    const auto nIt = std::remove(nReceivers.begin(), nReceivers.end(), nReceiver);
    nReceivers.erase(nIt, nReceivers.end());
}

bool NCInputManager::nDispatchEvent(const NSEvent& nEvent) {
    std::vector<NIEventReceiver*> nReceiversSnapshot;
    {
        std::scoped_lock nLock(nMutex);
        nApplyEventToState(nEvent);
        nReceiversSnapshot = nReceivers;
    }

    for (NIEventReceiver* nReceiver : nReceiversSnapshot) {
        if (nReceiver != nullptr && nReceiver->nOnEvent(nEvent)) {
            return true;
        }
    }

    return false;
}

bool NCInputManager::nProcessAndroidInput(AInputEvent* nAndroidEvent) {
#ifdef __ANDROID__
    if (nAndroidEvent == nullptr) {
        return false;
    }

    const auto nType = AInputEvent_getType(nAndroidEvent);
    if (nType == AINPUT_EVENT_TYPE_KEY) {
        NSKeyInput nKeyInput{};
        nKeyInput.nPressedDown = (AKeyEvent_getAction(nAndroidEvent) == AKEY_EVENT_ACTION_DOWN);

        switch (AKeyEvent_getKeyCode(nAndroidEvent)) {
            case AKEYCODE_BACK:
                nKeyInput.nKey = NKEY_CODE::NKEY_ANDROID_BACK;
                break;
            case AKEYCODE_MENU:
                nKeyInput.nKey = NKEY_CODE::NKEY_ANDROID_MENU;
                break;
            case AKEYCODE_VOLUME_UP:
                nKeyInput.nKey = NKEY_CODE::NKEY_ANDROID_VOLUME_UP;
                break;
            case AKEYCODE_VOLUME_DOWN:
                nKeyInput.nKey = NKEY_CODE::NKEY_ANDROID_VOLUME_DOWN;
                break;
            default:
                nKeyInput.nKey = NKEY_CODE::NKEY_UNKNOWN;
                break;
        }

        NSEvent nEvent{N_EVENT_TYPE::N_EVENT_KEY, nKeyInput};
        return nDispatchEvent(nEvent);
    }

    if (nType == AINPUT_EVENT_TYPE_MOTION) {
        const std::int32_t nAction = AMotionEvent_getAction(nAndroidEvent);
        const std::int32_t nMaskedAction = nAction & AMOTION_EVENT_ACTION_MASK;
        const std::size_t nPointerCount = static_cast<std::size_t>(AMotionEvent_getPointerCount(nAndroidEvent));

        for (std::size_t nPointerIndex = 0; nPointerIndex < nPointerCount; ++nPointerIndex) {
            NSTouchInput nTouch{};
            nTouch.nPointerId = AMotionEvent_getPointerId(nAndroidEvent, nPointerIndex);
            nTouch.nX = AMotionEvent_getX(nAndroidEvent, nPointerIndex);
            nTouch.nY = AMotionEvent_getY(nAndroidEvent, nPointerIndex);
            nTouch.nPressure = AMotionEvent_getPressure(nAndroidEvent, nPointerIndex);
            nTouch.nNormalizedX = nTouch.nX;
            nTouch.nNormalizedY = nTouch.nY;

            switch (nMaskedAction) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                    nTouch.nPhase = N_TOUCH_PHASE::N_TOUCH_DOWN;
                    break;
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:
                    nTouch.nPhase = N_TOUCH_PHASE::N_TOUCH_UP;
                    break;
                case AMOTION_EVENT_ACTION_MOVE:
                    nTouch.nPhase = N_TOUCH_PHASE::N_TOUCH_MOVE;
                    break;
                default:
                    nTouch.nPhase = N_TOUCH_PHASE::N_TOUCH_CANCEL;
                    break;
            }

            NSEvent nEvent{N_EVENT_TYPE::N_EVENT_TOUCH, nTouch};
            if (nDispatchEvent(nEvent)) {
                return true;
            }
        }
    }
#endif
    (void)nAndroidEvent;
    return false;
}

bool NCInputManager::nIsKeyDown(NKEY_CODE nKey) const {
    std::scoped_lock nLock(nMutex);
    return nKeysDown.test(nKeyToIndex(nKey));
}

bool NCInputManager::nWasKeyPressedThisFrame(NKEY_CODE nKey) const {
    std::scoped_lock nLock(nMutex);
    return nKeysPressedThisFrame.test(nKeyToIndex(nKey));
}

void NCInputManager::nBeginFrame() {
    std::scoped_lock nLock(nMutex);
    nKeysPressedThisFrame.reset();
}

std::array<std::int32_t, 2> NCInputManager::nGetMousePosition() const {
    std::scoped_lock nLock(nMutex);
    return nMousePosition;
}

std::unordered_map<std::int32_t, NSTouchInput> NCInputManager::nGetActiveTouches() const {
    std::scoped_lock nLock(nMutex);
    return nActiveTouches;
}

std::size_t NCInputManager::nKeyToIndex(NKEY_CODE nKey) noexcept {
    const auto nRaw = static_cast<std::size_t>(nKey);
    return std::min<std::size_t>(nRaw, NMAX_KEY_CODES - 1U);
}

void NCInputManager::nApplyEventToState(const NSEvent& nEvent) {
    switch (nEvent.nType) {
        case N_EVENT_TYPE::N_EVENT_KEY: {
            const auto& nKeyInput = std::get<NSKeyInput>(nEvent.nPayload);
            const auto nIndex = nKeyToIndex(nKeyInput.nKey);
            if (nKeyInput.nPressedDown) {
                if (!nKeysDown.test(nIndex)) {
                    nKeysPressedThisFrame.set(nIndex, true);
                }
                nKeysDown.set(nIndex, true);
            } else {
                nKeysDown.set(nIndex, false);
            }
            break;
        }
        case N_EVENT_TYPE::N_EVENT_MOUSE: {
            const auto& nMouseInput = std::get<NSMouseInput>(nEvent.nPayload);
            nMousePosition = {nMouseInput.nX, nMouseInput.nY};
            break;
        }
        case N_EVENT_TYPE::N_EVENT_TOUCH: {
            const auto& nTouchInput = std::get<NSTouchInput>(nEvent.nPayload);
            if (nTouchInput.nPhase == N_TOUCH_PHASE::N_TOUCH_UP || nTouchInput.nPhase == N_TOUCH_PHASE::N_TOUCH_CANCEL) {
                nActiveTouches.erase(nTouchInput.nPointerId);
            } else {
                nActiveTouches[nTouchInput.nPointerId] = nTouchInput;
            }
            break;
        }
        case N_EVENT_TYPE::N_EVENT_GUI:
        case N_EVENT_TYPE::N_EVENT_USER:
            break;
    }
}

} // namespace N
