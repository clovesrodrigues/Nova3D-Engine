#pragma once

#include "NKeyCodes.hpp"

#include <cstdint>
#include <variant>

namespace N {

struct NSTouchInput {
    std::int32_t nPointerId{0};
    float nX{0.0F};
    float nY{0.0F};
    float nNormalizedX{0.0F};
    float nNormalizedY{0.0F};
    float nPressure{0.0F};
    N_TOUCH_PHASE nPhase{N_TOUCH_PHASE::N_TOUCH_DOWN};
};

struct NSKeyInput {
    NKEY_CODE nKey{NKEY_CODE::NKEY_UNKNOWN};
    char32_t nUnicode{U'\0'};
    bool nPressedDown{false};
    bool nShift{false};
    bool nControl{false};
};

struct NSMouseInput {
    std::int32_t nX{0};
    std::int32_t nY{0};
    N_MOUSE_INPUT_EVENT nMouseEvent{N_MOUSE_INPUT_EVENT::N_MOUSE_MOVED};
    float nWheelDelta{0.0F};
};

enum class N_GUI_EVENT_TYPE : std::uint8_t {
    N_GUI_CLICKED = 0,
    N_GUI_FOCUSED,
    N_GUI_TEXT_CHANGED
};

struct NSGUIEvent {
    std::uint64_t nElementId{0};
    N_GUI_EVENT_TYPE nGuiEventType{N_GUI_EVENT_TYPE::N_GUI_CLICKED};
};

struct NSUserEvent {
    std::uint64_t nUserEventId{0};
    std::uintptr_t nData1{0};
    std::uintptr_t nData2{0};
    void* nUserPointer1{nullptr};
    void* nUserPointer2{nullptr};
};

enum class N_EVENT_TYPE : std::uint8_t {
    N_EVENT_KEY = 0,
    N_EVENT_MOUSE,
    N_EVENT_TOUCH,
    N_EVENT_GUI,
    N_EVENT_USER
};

using NSEventPayload = std::variant<NSKeyInput, NSMouseInput, NSTouchInput, NSGUIEvent, NSUserEvent>;

struct NSEvent {
    N_EVENT_TYPE nType{N_EVENT_TYPE::N_EVENT_USER};
    NSEventPayload nPayload{NSUserEvent{}};
};

} // namespace N
