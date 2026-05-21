#pragma once

#include "AI/Entities/NIAIEntity.hpp"

namespace NAI {

class NIPlayerAIEntity : public NIAIEntity {
public:
    ~NIPlayerAIEntity() override = default;
    virtual void nSyncFromExternalTransform(const NVector3& nPosition, const NVector3& nRotation) noexcept = 0;
};

class NCPlayerAIEntity final : public NIPlayerAIEntity {
public:
    explicit NCPlayerAIEntity(int nId);
    ~NCPlayerAIEntity() override = default;

    void nUpdate(float nDeltaTime) override;
    int nGetId() const noexcept override;
    int nGetFactionId() const noexcept override;
    const NVector3& nGetPosition() const noexcept override;
    const NVector3& nGetRotation() const noexcept override;
    void* nGetUserData() const noexcept override;

    void nSetFactionId(int nFactionId) noexcept override;
    void nSetPosition(const NVector3& nPosition) noexcept override;
    void nSetRotation(const NVector3& nRotation) noexcept override;
    void nSetUserData(void* nUserData) noexcept override;
    void nSyncFromExternalTransform(const NVector3& nPosition, const NVector3& nRotation) noexcept override;

private:
    int nId_{0};
    int nFactionId_{0};
    NVector3 nPosition_{};
    NVector3 nRotation_{};
    void* nUserData_{nullptr};
};

} // namespace NAI
