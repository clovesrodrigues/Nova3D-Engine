#pragma once

#include <memory>
#include <vector>

#include "AI/Entities/NICombatNPC.hpp"
#include "AI/Entities/NIAISensor.hpp"
#include "AI/Entities/NIPlayerAIEntity.hpp"
#include "AI/Pathfinding/NCAStarPathFinder.hpp"

namespace NAI {

class NIAIManager {
public:
    virtual ~NIAIManager() = default;

    virtual void nUpdate(float nDeltaTime) = 0;
    virtual void nClear() = 0;

    [[nodiscard]] virtual std::shared_ptr<NIPlayerAIEntity> nCreatePlayerEntity(int nId) = 0;
    [[nodiscard]] virtual std::shared_ptr<NINPC> nCreatePathFindingNPC(int nId, std::shared_ptr<NIPathFinder> nPathFinder) = 0;
    [[nodiscard]] virtual std::shared_ptr<NICombatNPC> nCreateCombatNPC(int nId,
                                                                         std::shared_ptr<NIPathFinder> nPathFinder,
                                                                         std::shared_ptr<NIFieldOfView> nFov) = 0;
    [[nodiscard]] virtual std::shared_ptr<NIAISensor> nCreateEntryExitSensor(const NAABBox& nBoundingBox) = 0;
    [[nodiscard]] virtual std::shared_ptr<NSWaypointGroup> nCreateWaypointGroup() = 0;
    [[nodiscard]] virtual std::shared_ptr<NIPathFinder> nCreateAStarPathFinder(std::shared_ptr<NSWaypointGroup> nWaypointGroup) = 0;

    [[nodiscard]] virtual const std::vector<std::shared_ptr<NIAIEntity>>& nGetEntities() const noexcept = 0;
    [[nodiscard]] virtual const std::vector<std::shared_ptr<NSWaypointGroup>>& nGetWaypointGroups() const noexcept = 0;
};

class NCAIManager final : public NIAIManager {
public:
    NCAIManager() = default;
    ~NCAIManager() override = default;

    void nUpdate(float nDeltaTime) override;
    void nClear() override;

    std::shared_ptr<NIPlayerAIEntity> nCreatePlayerEntity(int nId) override;
    std::shared_ptr<NINPC> nCreatePathFindingNPC(int nId, std::shared_ptr<NIPathFinder> nPathFinder) override;
    std::shared_ptr<NICombatNPC> nCreateCombatNPC(int nId,
                                                   std::shared_ptr<NIPathFinder> nPathFinder,
                                                   std::shared_ptr<NIFieldOfView> nFov) override;
    std::shared_ptr<NIAISensor> nCreateEntryExitSensor(const NAABBox& nBoundingBox) override;
    std::shared_ptr<NSWaypointGroup> nCreateWaypointGroup() override;
    std::shared_ptr<NIPathFinder> nCreateAStarPathFinder(std::shared_ptr<NSWaypointGroup> nWaypointGroup) override;

    const std::vector<std::shared_ptr<NIAIEntity>>& nGetEntities() const noexcept override;
    const std::vector<std::shared_ptr<NSWaypointGroup>>& nGetWaypointGroups() const noexcept override;

private:
    std::vector<std::shared_ptr<NIAIEntity>> nEntities_{};
    std::vector<std::shared_ptr<NSWaypointGroup>> nWaypointGroups_{};
    std::vector<std::shared_ptr<NIAISensor>> nSensors_{};
};

} // namespace NAI
