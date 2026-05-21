#pragma once

#include <list>
#include <memory>

#include "AI/Entities/NIAIEntity.hpp"
#include "AI/Pathfinding/NIPathFinder.hpp"

namespace NAI {

class NINPC : public NIAIEntity {
public:
    enum class NState { N_STATE_WAITING, N_STATE_MOVING, N_STATE_FOLLOWING_PATH, N_STATE_CHASING, N_STATE_ATTACKING };

    ~NINPC() override = default;

    virtual bool nSetPathTo(const std::shared_ptr<NIWaypoint>& nStart,
                            const std::shared_ptr<NIWaypoint>& nGoal) = 0;
    virtual void nStop() noexcept = 0;

    [[nodiscard]] virtual NState nGetState() const noexcept = 0;
    [[nodiscard]] virtual float nGetSpeed() const noexcept = 0;
    [[nodiscard]] virtual float nGetWaypointTolerance() const noexcept = 0;
    [[nodiscard]] virtual const std::list<std::shared_ptr<NIWaypoint>>& nGetCurrentPath() const noexcept = 0;

    virtual void nSetSpeed(float nSpeed) noexcept = 0;
    virtual void nSetWaypointTolerance(float nTolerance) noexcept = 0;
};

class NCPathFindingNPC : public NINPC {
public:
    NCPathFindingNPC(int nId, std::shared_ptr<NIPathFinder> nPathFinder);
    ~NCPathFindingNPC() override = default;

    void nUpdate(float nDeltaTime) override;
    bool nSetPathTo(const std::shared_ptr<NIWaypoint>& nStart,
                    const std::shared_ptr<NIWaypoint>& nGoal) override;
    void nStop() noexcept override;

    int nGetId() const noexcept override;
    int nGetFactionId() const noexcept override;
    const NVector3& nGetPosition() const noexcept override;
    const NVector3& nGetRotation() const noexcept override;
    void* nGetUserData() const noexcept override;
    NState nGetState() const noexcept override;
    float nGetSpeed() const noexcept override;
    float nGetWaypointTolerance() const noexcept override;
    const std::list<std::shared_ptr<NIWaypoint>>& nGetCurrentPath() const noexcept override;

    void nSetFactionId(int nFactionId) noexcept override;
    void nSetPosition(const NVector3& nPosition) noexcept override;
    void nSetRotation(const NVector3& nRotation) noexcept override;
    void nSetUserData(void* nUserData) noexcept override;
    void nSetSpeed(float nSpeed) noexcept override;
    void nSetWaypointTolerance(float nTolerance) noexcept override;

protected:
    virtual void nOnPathBlocked();

    int nId_{0};
    int nFactionId_{0};
    NVector3 nPosition_{};
    NVector3 nRotation_{};
    void* nUserData_{nullptr};

    NState nState_{NState::N_STATE_WAITING};
    std::shared_ptr<NIPathFinder> nPathFinder_{};
    std::list<std::shared_ptr<NIWaypoint>> nCurrentPath_{};
    float nSpeed_{3.5f};
    float nWaypointTolerance_{0.25f};
};

} // namespace NAI
