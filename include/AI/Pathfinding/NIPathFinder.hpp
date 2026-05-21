#pragma once

#include <list>
#include <memory>

#include "AI/Pathfinding/NSWaypointGroup.hpp"

namespace NAI {

class NIPathFinder {
public:
    virtual ~NIPathFinder() = default;

    virtual bool nFindPath(const std::shared_ptr<NIWaypoint>& nStart,
                           const std::shared_ptr<NIWaypoint>& nGoal,
                           std::list<std::shared_ptr<NIWaypoint>>& nOutPath) = 0;

    virtual void nSetWaypointGroup(std::shared_ptr<NSWaypointGroup> nWaypointGroup) = 0;
    [[nodiscard]] virtual std::shared_ptr<NSWaypointGroup> nGetWaypointGroup() const = 0;
};

} // namespace NAI
