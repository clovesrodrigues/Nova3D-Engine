#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace NEngine {

struct NVector2 {
    float nX{0.0F};
    float nY{0.0F};
};

struct NVector3 {
    float nX{0.0F};
    float nY{0.0F};
    float nZ{0.0F};

    [[nodiscard]] NVector3 nAdd(const NVector3& nOther) const noexcept {
        return {nX + nOther.nX, nY + nOther.nY, nZ + nOther.nZ};
    }

    [[nodiscard]] NVector3 nSub(const NVector3& nOther) const noexcept {
        return {nX - nOther.nX, nY - nOther.nY, nZ - nOther.nZ};
    }

    [[nodiscard]] NVector3 nScale(float nScalar) const noexcept {
        return {nX * nScalar, nY * nScalar, nZ * nScalar};
    }

    [[nodiscard]] float nLength() const noexcept {
        return std::sqrt((nX * nX) + (nY * nY) + (nZ * nZ));
    }

    [[nodiscard]] NVector3 nNormalized() const noexcept {
        const float nLen = nLength();
        return (nLen > 0.0F) ? nScale(1.0F / nLen) : NVector3{};
    }

    [[nodiscard]] static NVector3 nLerp(const NVector3& nA, const NVector3& nB, float nT) noexcept {
        const float nClamped = std::clamp(nT, 0.0F, 1.0F);
        return {nA.nX + ((nB.nX - nA.nX) * nClamped), nA.nY + ((nB.nY - nA.nY) * nClamped), nA.nZ + ((nB.nZ - nA.nZ) * nClamped)};
    }
};

struct NColor {
    float nR{1.0F};
    float nG{1.0F};
    float nB{1.0F};
    float nA{1.0F};

    [[nodiscard]] static NColor nLerp(const NColor& nA, const NColor& nB, float nT) noexcept {
        const float nClamped = std::clamp(nT, 0.0F, 1.0F);
        return {
            nA.nR + ((nB.nR - nA.nR) * nClamped),
            nA.nG + ((nB.nG - nA.nG) * nClamped),
            nA.nB + ((nB.nB - nA.nB) * nClamped),
            nA.nA + ((nB.nA - nA.nA) * nClamped)
        };
    }
};

struct NAABBox {
    NVector3 nMin;
    NVector3 nMax;
};

struct NMatrix4 {
    std::array<float, 16> nM{1, 0, 0, 0,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             0, 0, 0, 1};
};

} // namespace NEngine
