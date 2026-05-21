#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "AI/Pathfinding/NIWaypoint.hpp"

namespace NAI {

class NSWaypointGroup {
public:
    [[nodiscard]] std::shared_ptr<NIWaypoint> nCreateWaypoint(int nId, const NVector3& nPosition);
    [[nodiscard]] std::shared_ptr<NIWaypoint> nGetWaypointById(int nId) const;
    [[nodiscard]] std::shared_ptr<NIWaypoint> nGetClosestWaypoint(const NVector3& nPosition) const;
    [[nodiscard]] const std::vector<std::shared_ptr<NIWaypoint>>& nGetAllWaypoints() const noexcept;

    void nConnectBidirectional(int nWaypointAId, int nWaypointBId, float nCost = -1.0f);
    void nClear();

private:
    std::unordered_map<int, std::shared_ptr<NIWaypoint>> nWaypointsById_{};
    std::vector<std::shared_ptr<NIWaypoint>> nWaypoints_{};
};

} // namespace NAI
