#pragma once

#include <functional>
#include <memory>

#include "AI/Entities/NINPC.hpp"
#include "AI/Vision/NIFieldOfView.hpp"

namespace NAI {

class NICombatNPC : public NINPC {
public:
    ~NICombatNPC() override = default;

    virtual void nSetFieldOfView(std::shared_ptr<NIFieldOfView> nFov) = 0;
    virtual void nSetTarget(std::weak_ptr<NIAIEntity> nTarget) = 0;
    virtual void nSetAttackRange(float nAttackRange) noexcept = 0;
    virtual void nSetAttackCooldown(float nAttackCooldown) noexcept = 0;
    virtual void nSetAttackCallback(std::function<void(std::shared_ptr<NIAIEntity>)> nOnAttack) = 0;
};

class NCCombatNPC final : public NCPathFindingNPC, public NICombatNPC {
public:
    NCCombatNPC(int nId,
                std::shared_ptr<NIPathFinder> nPathFinder,
                std::shared_ptr<NIFieldOfView> nFov);

    void nUpdate(float nDeltaTime) override;
    void nSetFieldOfView(std::shared_ptr<NIFieldOfView> nFov) override;
    void nSetTarget(std::weak_ptr<NIAIEntity> nTarget) override;
    void nSetAttackRange(float nAttackRange) noexcept override;
    void nSetAttackCooldown(float nAttackCooldown) noexcept override;
    void nSetAttackCallback(std::function<void(std::shared_ptr<NIAIEntity>)> nOnAttack) override;

private:
    std::shared_ptr<NIFieldOfView> nFOV_{};
    std::weak_ptr<NIAIEntity> nTarget_{};
    float nAttackRange_{2.0f};
    float nAttackCooldown_{1.0f};
    float nCooldownTimer_{0.0f};
    std::function<void(std::shared_ptr<NIAIEntity>)> nOnAttack_{};
};

} // namespace NAI
