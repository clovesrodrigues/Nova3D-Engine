#pragma once

#include "NSEvent.hpp"

namespace N {

class NIEventReceiver {
public:
    virtual ~NIEventReceiver();

    virtual bool nOnEvent(const NSEvent& nEvent) noexcept = 0;
};

} // namespace N
