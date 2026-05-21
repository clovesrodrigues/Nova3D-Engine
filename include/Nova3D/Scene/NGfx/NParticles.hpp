#pragma once

#include "NMathTypes.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace NEngine {

class NIAnimatedMeshSceneNode;
class NIMesh;

struct NSParticle {
    NVector3 nPosition{};
    NVector3 nVelocity{};
    NColor nColor{};
    float nSize{1.0F};
    uint32_t nStartTimeMs{0U};
    uint32_t nLifeTimeMs{1000U};
};

class NIParticleAffector {
public:
    virtual ~NIParticleAffector() = default;
    virtual void nAffect(std::vector<NSParticle>& nParticles, uint32_t nTimeMs) = 0;

    [[nodiscard]] bool nIsEnabled() const noexcept { return nEnabled; }
    void nSetEnabled(bool nInEnabled) noexcept { nEnabled = nInEnabled; }

protected:
    bool nEnabled{true};
};

class NIParticleAttractionAffector final : public NIParticleAffector {
public:
    void nAffect(std::vector<NSParticle>& nParticles, uint32_t nTimeMs) override;

private:
    NVector3 nPoint{};
    float nSpeed{1.0F};
    bool nAttract{true};
};

class NIParticleFadeOutAffector final : public NIParticleAffector {
public:
    void nAffect(std::vector<NSParticle>& nParticles, uint32_t nTimeMs) override;

private:
    NColor nTargetColor{1.0F, 1.0F, 1.0F, 0.0F};
    float nFadeTime{1000.0F};
};

class NIParticleGravityAffector final : public NIParticleAffector {
public:
    void nAffect(std::vector<NSParticle>& nParticles, uint32_t nTimeMs) override;

private:
    NVector3 nGravity{0.0F, -9.8F, 0.0F};
};

class NIParticleRotationAffector final : public NIParticleAffector {
public:
    void nAffect(std::vector<NSParticle>& nParticles, uint32_t nTimeMs) override;

private:
    NVector3 nPivotPoint{};
    NVector3 nRotationSpeed{};
};

class NIParticleEmitter {
public:
    virtual ~NIParticleEmitter() = default;
    virtual void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) = 0;

protected:
    NVector3 nDirection{0.0F, 1.0F, 0.0F};
    uint32_t nMinParticlesPerSecond{10U};
    uint32_t nMaxParticlesPerSecond{20U};
    NColor nMinStartColor{};
    NColor nMaxStartColor{};
    uint32_t nMinLifeTime{1000U};
    uint32_t nMaxLifeTime{3000U};
};

class NIParticleAnimatedMeshSceneNodeEmitter final : public NIParticleEmitter {
public:
    void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) override;

private:
    std::weak_ptr<NIAnimatedMeshSceneNode> nAnimatedMeshNode;
};

class NIParticleBoxEmitter final : public NIParticleEmitter {
public:
    void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) override;

private:
    NAABBox nBox{};
};

class NIParticleCylinderEmitter final : public NIParticleEmitter {
public:
    void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) override;

private:
    NVector3 nCenter{};
    float nRadius{1.0F};
    float nLength{1.0F};
};

class NIParticleMeshEmitter final : public NIParticleEmitter {
public:
    void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) override;

private:
    std::shared_ptr<NIMesh> nMesh;
};

class NIParticleRingEmitter final : public NIParticleEmitter {
public:
    void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) override;

private:
    NVector3 nCenter{};
    float nRadius{1.0F};
    float nRingThickness{0.25F};
};

class NIParticleSphereEmitter final : public NIParticleEmitter {
public:
    void nEmitt(uint32_t nTimeMs, uint32_t nTimeSinceLastCall, std::vector<NSParticle>& nOutParticles) override;

private:
    NVector3 nCenter{};
    float nRadius{1.0F};
};

} // namespace NEngine
