#pragma once

#include "NMathTypes.hpp"
#include "NParticles.hpp"
#include "NSceneNodeAnimator.hpp"

#include <memory>
#include <string>
#include <vector>

namespace NEngine {

class NIMesh;

/**
 * @brief Base node for the scene graph with shared ownership hierarchy.
 */
class NISceneNode : public std::enable_shared_from_this<NISceneNode> {
public:
    virtual ~NISceneNode() = default;

    virtual void nOnRegisterSceneNode();
    virtual void nUpdateAbsolutePosition();
    virtual void nRender() = 0;

    void nAddChild(const std::shared_ptr<NISceneNode>& nChild);
    void nAddAnimator(std::unique_ptr<NISceneNodeAnimator> nAnimator);

protected:
    std::weak_ptr<NISceneNode> nParent;
    std::vector<std::shared_ptr<NISceneNode>> nChildren;
    NVector3 nPosition{};
    NVector3 nRotation{};
    NVector3 nScale{1.0F, 1.0F, 1.0F};
    NMatrix4 nAbsoluteTransformation{};
    std::vector<std::unique_ptr<NISceneNodeAnimator>> nAnimators;
};

class NIAnimatedMeshSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NIBillboardSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NIBillboardTextSceneNode final : public NISceneNode {
public:
    void nRender() override;

private:
    std::string nText;
};

class NIBoneSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NICameraSceneNode final : public NISceneNode {
public:
    void nRender() override;

private:
    NMatrix4 nViewMatrix{};
    NMatrix4 nProjectionMatrix{};
};

class NIDummyTransformationSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NILightSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NIMeshSceneNode final : public NISceneNode {
public:
    void nRender() override;

private:
    std::shared_ptr<NIMesh> nMesh;
};

class NIParticleSystemSceneNode final : public NISceneNode {
public:
    void nRender() override;

private:
    std::unique_ptr<NIParticleEmitter> nEmitter;
    std::vector<std::unique_ptr<NIParticleAffector>> nAffectors;
    std::vector<NSParticle> nParticles;
};

class NIShadowVolumeSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NITerrainSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

class NITextSceneNode final : public NISceneNode {
public:
    void nRender() override;

private:
    std::string nText;
};

class NIVolumeLightSceneNode final : public NISceneNode {
public:
    void nRender() override;
};

} // namespace NEngine
