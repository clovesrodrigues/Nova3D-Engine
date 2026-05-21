#pragma once

#include <memory>
#include <vector>

#include "AI/Math/NMathTypes.hpp"

namespace NAI {

class NIWaypoint;

struct NSNeighbour {
    std::weak_ptr<NIWaypoint> nWaypoint;
    float nCost{0.0f};
};

class NIWaypoint {
public:
    virtual ~NIWaypoint() = default;

    [[nodiscard]] virtual int nGetId() const noexcept = 0;
    [[nodiscard]] virtual const NVector3& nGetPosition() const noexcept = 0;
    [[nodiscard]] virtual const std::vector<NSNeighbour>& nGetNeighbours() const noexcept = 0;

    virtual void nSetPosition(const NVector3& nPosition) noexcept = 0;
    virtual void nAddNeighbour(const std::shared_ptr<NIWaypoint>& nNeighbour, float nCost) = 0;
};

class NCWaypoint final : public NIWaypoint {
public:
    NCWaypoint(int nId, const NVector3& nPosition);
    ~NCWaypoint() override = default;

    int nGetId() const noexcept override;
    const NVector3& nGetPosition() const noexcept override;
    const std::vector<NSNeighbour>& nGetNeighbours() const noexcept override;

    void nSetPosition(const NVector3& nPosition) noexcept override;
    void nAddNeighbour(const std::shared_ptr<NIWaypoint>& nNeighbour, float nCost) override;

private:
    int nId_{0};
    NVector3 nPosition_{};
    std::vector<NSNeighbour> nNeighbours_{};
};

} // namespace NAI
