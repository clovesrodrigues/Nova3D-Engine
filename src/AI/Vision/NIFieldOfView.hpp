#pragma once

#include <memory>
#include <vector>

#include "AI/Entities/NIAIEntity.hpp"

namespace NAI {

class NIFieldOfView {
public:
    virtual ~NIFieldOfView() = default;

    [[nodiscard]] virtual bool nCanSee(const NIAIEntity& nObserver,
                                       const NIAIEntity& nTarget,
                                       const NVector3& nObserverForward) const = 0;

    [[nodiscard]] virtual std::vector<std::shared_ptr<NIAIEntity>> nScanVisibleTargets(
        const NIAIEntity& nObserver,
        const NVector3& nObserverForward,
        const std::vector<std::shared_ptr<NIAIEntity>>& nCandidates) const = 0;
};

class NCConeFieldOfView final : public NIFieldOfView {
public:
    NCConeFieldOfView(float nViewDistance, float nFieldOfViewAngleDegrees);

    bool nCanSee(const NIAIEntity& nObserver,
                 const NIAIEntity& nTarget,
                 const NVector3& nObserverForward) const override;

    std::vector<std::shared_ptr<NIAIEntity>> nScanVisibleTargets(
        const NIAIEntity& nObserver,
        const NVector3& nObserverForward,
        const std::vector<std::shared_ptr<NIAIEntity>>& nCandidates) const override;

    void nSetViewDistance(float nDistance) noexcept;
    void nSetFieldOfViewAngle(float nAngleDegrees) noexcept;

private:
    float nViewDistance_{20.0f};
    float nFieldOfViewAngleDegrees_{60.0f};
};

} // namespace NAI
