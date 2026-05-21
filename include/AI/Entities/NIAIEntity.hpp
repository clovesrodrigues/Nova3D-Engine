#pragma once

#include "AI/Math/NMathTypes.hpp"

namespace NAI {

class NIAIEntity {
public:
    virtual ~NIAIEntity() = default;

    virtual void nUpdate(float nDeltaTime) = 0;

    [[nodiscard]] virtual int nGetId() const noexcept = 0;
    [[nodiscard]] virtual int nGetFactionId() const noexcept = 0;
    [[nodiscard]] virtual const NVector3& nGetPosition() const noexcept = 0;
    [[nodiscard]] virtual const NVector3& nGetRotation() const noexcept = 0;
    [[nodiscard]] virtual void* nGetUserData() const noexcept = 0;

    virtual void nSetFactionId(int nFactionId) noexcept = 0;
    virtual void nSetPosition(const NVector3& nPosition) noexcept = 0;
    virtual void nSetRotation(const NVector3& nRotation) noexcept = 0;
    virtual void nSetUserData(void* nUserData) noexcept = 0;
};

} // namespace NAI
