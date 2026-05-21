#pragma once

#include <functional>
#include <memory>
#include <unordered_set>
#include <vector>

#include "AI/Entities/NIAIEntity.hpp"

namespace NAI {

class NIAISensor {
public:
    virtual ~NIAISensor() = default;

    virtual void nUpdate(const std::vector<std::shared_ptr<NIAIEntity>>& nEntities) = 0;
    virtual void nSetOnEnterCallback(std::function<void(std::shared_ptr<NIAIEntity>)> nOnEnter) = 0;
    virtual void nSetOnExitCallback(std::function<void(std::shared_ptr<NIAIEntity>)> nOnExit) = 0;
};

class NCEntryExitSensor final : public NIAISensor {
public:
    explicit NCEntryExitSensor(const NAABBox& nBoundingBox);

    void nUpdate(const std::vector<std::shared_ptr<NIAIEntity>>& nEntities) override;
    void nSetOnEnterCallback(std::function<void(std::shared_ptr<NIAIEntity>)> nOnEnter) override;
    void nSetOnExitCallback(std::function<void(std::shared_ptr<NIAIEntity>)> nOnExit) override;
    void nSetBoundingBox(const NAABBox& nBoundingBox) noexcept;

private:
    NAABBox nBoundingBox_{};
    std::unordered_set<int> nEntitiesInside_{};
    std::function<void(std::shared_ptr<NIAIEntity>)> nOnEnter_{};
    std::function<void(std::shared_ptr<NIAIEntity>)> nOnExit_{};
};

} // namespace NAI
