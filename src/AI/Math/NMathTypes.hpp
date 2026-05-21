#pragma once

#include <algorithm>
#include <cmath>

namespace NAI {

struct NVector3 {
    float nX{0.0f};
    float nY{0.0f};
    float nZ{0.0f};

    [[nodiscard]] float nLength() const noexcept { return std::sqrt(nX * nX + nY * nY + nZ * nZ); }
    [[nodiscard]] float nLengthSquared() const noexcept { return nX * nX + nY * nY + nZ * nZ; }

    [[nodiscard]] NVector3 nNormalized() const noexcept {
        const float nLen = nLength();
        if (nLen <= 0.000001f) {
            return {};
        }
        return {nX / nLen, nY / nLen, nZ / nLen};
    }

    [[nodiscard]] static float nDot(const NVector3& nA, const NVector3& nB) noexcept {
        return nA.nX * nB.nX + nA.nY * nB.nY + nA.nZ * nB.nZ;
    }

    [[nodiscard]] static float nDistance(const NVector3& nA, const NVector3& nB) noexcept {
        return (nA - nB).nLength();
    }

    [[nodiscard]] NVector3 operator+(const NVector3& nOther) const noexcept { return {nX + nOther.nX, nY + nOther.nY, nZ + nOther.nZ}; }
    [[nodiscard]] NVector3 operator-(const NVector3& nOther) const noexcept { return {nX - nOther.nX, nY - nOther.nY, nZ - nOther.nZ}; }
    [[nodiscard]] NVector3 operator*(float nScalar) const noexcept { return {nX * nScalar, nY * nScalar, nZ * nScalar}; }
};

struct NColor {
    float nR{1.0f};
    float nG{1.0f};
    float nB{1.0f};
    float nA{1.0f};
};

struct NAABBox {
    NVector3 nMin{};
    NVector3 nMax{};

    [[nodiscard]] bool nContains(const NVector3& nPoint) const noexcept {
        return nPoint.nX >= nMin.nX && nPoint.nX <= nMax.nX &&
               nPoint.nY >= nMin.nY && nPoint.nY <= nMax.nY &&
               nPoint.nZ >= nMin.nZ && nPoint.nZ <= nMax.nZ;
    }
};

} // namespace NAI
