#pragma once

#include <list>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "AI/Pathfinding/NIPathFinder.hpp"

namespace NAI {

class NCAStarPathFinder final : public NIPathFinder {
public:
    enum class NHeuristicMode { N_EUCLIDEAN, N_MANHATTAN };

    struct NSSearchNode {
        int nWaypointId{0};
        float nG{0.0f};
        float nH{0.0f};
        float nF{0.0f};
        int nParentWaypointId{-1};
    };

    explicit NCAStarPathFinder(std::shared_ptr<NSWaypointGroup> nWaypointGroup = nullptr);

    bool nFindPath(const std::shared_ptr<NIWaypoint>& nStart,
                   const std::shared_ptr<NIWaypoint>& nGoal,
                   std::list<std::shared_ptr<NIWaypoint>>& nOutPath) override;

    void nSetWaypointGroup(std::shared_ptr<NSWaypointGroup> nWaypointGroup) override;
    [[nodiscard]] std::shared_ptr<NSWaypointGroup> nGetWaypointGroup() const override;
    void nSetHeuristicMode(NHeuristicMode nMode) noexcept;

private:
    [[nodiscard]] float nHeuristic(const NVector3& nA, const NVector3& nB) const noexcept;
    void nReconstructPath(int nGoalId,
                          const std::unordered_map<int, NSSearchNode>& nVisited,
                          std::list<std::shared_ptr<NIWaypoint>>& nOutPath) const;

    std::shared_ptr<NSWaypointGroup> nWaypointGroup_{};
    NHeuristicMode nMode_{NHeuristicMode::N_EUCLIDEAN};
};

// Complexidade esperada do A*: O((V + E) * log(V)) no caso médio com heap binária.

} // namespace NAI
