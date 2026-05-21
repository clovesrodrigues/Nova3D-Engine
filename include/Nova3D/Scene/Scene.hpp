#pragma once
#include <memory>
#include <vector>
#include <string>
#include <Nova3D/Core/Math/Math.hpp>
#include <Nova3D/Scene/IMesh.hpp>
#include <Nova3D/Graphics/IRenderer.hpp>
#include <Nova3D/Spatial/SpatialPartition.hpp>
#include <Nova3D/Terrain/Terrain.hpp>
#include <Nova3D/Assets/Importers/NModelImporter.hpp>

namespace nova3d::graphics { class ForwardRenderer; }
namespace nova3d::assets { struct ModelAsset; }
namespace nova3d::scene {

enum class NPivotMode { MeshCenter, BottomCenter, TopCenter, Custom };

struct NPlacementOptions {
    math::Vector3 initialPosition{};
    math::Vector3 initialRotationEuler{};
    math::Vector3 initialScale{1,1,1};
    NPivotMode pivotMode = NPivotMode::MeshCenter;
    math::Vector3 customPivotOffset{};
    bool useLookAt = false;
    math::Vector3 lookAtTarget{};
    math::Vector3 upVector{0,1,0};
    bool alignToGround = false;
    std::string debugName;
};

struct NSpawnReport {
    bool success = false;
    std::string assetPath;
    uint32_t createdNodeCount = 0;
    math::AABB modelBounds{};
    math::Vector3 computedPivotWorld{};
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

math::Vector3 computePivotOffset(const math::AABB& bounds, NPivotMode mode, const math::Vector3& customPivotOffset);

struct Light { math::Vector3 color{1,1,1}; float intensity=1.0F; virtual ~Light()=default; };
struct DirectionalLight final : Light { math::Vector3 direction{0,-1,0}; };
struct PointLight final : Light { float range=10.0F; math::Vector3 attenuation{1.0F,0.09F,0.032F}; };
struct SpotLight final : Light { math::Vector3 direction{0,-1,0}; float range=10.0F; float innerCone=0.7F; float outerCone=0.9F; };

enum class RenderMobility { Static, Dynamic };
struct MeshDrawPacket { std::shared_ptr<IMeshBuffer> meshBuffer; math::Matrix4 world; std::size_t subMeshIndex=0; RenderMobility mobility=RenderMobility::Dynamic; };
struct RenderItem { std::shared_ptr<IMeshBuffer> meshBuffer; math::Matrix4 world; std::size_t subMeshIndex=0; RenderMobility mobility=RenderMobility::Dynamic;};
struct LightingContext { std::vector<DirectionalLight> directionalLights; std::vector<PointLight> pointLights; };

enum class NTransformSpace { Local, World };
class NTransformGizmo {
public:
    explicit NTransformGizmo(std::shared_ptr<ISceneNode> node):m_node(std::move(node)){}
    void setTransformSpace(NTransformSpace space){ m_space = space; }
    NTransformSpace transformSpace() const { return m_space; }
    void setPosition(const math::Vector3& p);
    void setRotation(const math::Quaternion& q);
    void setScale(const math::Vector3& s);
    void translate(const math::Vector3& delta);
    void rotate(const math::Quaternion& delta);
    void scaleBy(const math::Vector3& factor);
private:
    std::shared_ptr<ISceneNode> m_node;
    NTransformSpace m_space = NTransformSpace::Local;
};

class SceneManager;
class ISceneNode : public std::enable_shared_from_this<ISceneNode> { public: virtual ~ISceneNode() = default; virtual void onUpdate(float dt) = 0; virtual void onRegister(SceneManager& mgr) = 0; virtual math::AABB worldBounds() const = 0; math::Transform& transform() { return m_transform; } const math::Transform& transform() const { return m_transform; } void setVisible(bool visible) { m_visible = visible; } bool visible() const { return m_visible; } void addChild(const std::shared_ptr<ISceneNode>& child); const std::vector<std::shared_ptr<ISceneNode>>& children() const { return m_children; } const math::Matrix4& worldMatrix() const { return m_world; } void updateWorldRecursive(const math::Matrix4& parentWorld); protected: math::Transform m_transform{}; math::Matrix4 m_world=math::Matrix4::identity(); bool m_visible=true; std::weak_ptr<ISceneNode> m_parent; std::vector<std::shared_ptr<ISceneNode>> m_children;};
class SceneNode : public ISceneNode { public: void onUpdate(float) override {} void onRegister(SceneManager&) override {} math::AABB worldBounds() const override { return {}; } };
class CameraSceneNode : public SceneNode { public: void setPerspective(float fovDeg, float aspect, float nearPlane, float farPlane); void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane); void lookAt(const math::Vector3& target, const math::Vector3& up = {0,1,0}); void setViewport(const graphics::Viewport& viewport){m_viewport=viewport;} const graphics::Viewport& viewport() const { return m_viewport; } void setRenderTarget(const std::shared_ptr<graphics::IRenderTarget>& target){m_target=target;} std::shared_ptr<graphics::IRenderTarget> renderTarget() const { return m_target; } const math::Matrix4& viewMatrix() const { return m_view; } const math::Matrix4& projectionMatrix() const { return m_proj; } math::Matrix4 viewProjectionMatrix() const { return m_proj*m_view; } math::Frustum frustum() const { return math::Frustum::fromViewProjection(viewProjectionMatrix()); } private: math::Matrix4 m_view=math::Matrix4::identity(); math::Matrix4 m_proj=math::Matrix4::identity(); graphics::Viewport m_viewport{0,0,1280,720}; std::shared_ptr<graphics::IRenderTarget> m_target; };
class MeshSceneNode : public SceneNode { public: explicit MeshSceneNode(std::shared_ptr<IMesh> mesh) : m_mesh(std::move(mesh)) {} void onRegister(SceneManager& mgr) override; math::AABB worldBounds() const override; private: std::shared_ptr<IMesh> m_mesh;};
class LightSceneNode : public SceneNode { public: explicit LightSceneNode(std::shared_ptr<Light> light):m_light(std::move(light)){} void onRegister(SceneManager& mgr) override; math::AABB worldBounds() const override; private: std::shared_ptr<Light> m_light;};
class TerrainSceneNode : public SceneNode { public: explicit TerrainSceneNode(terrain::TerrainPatch patch):m_patch(std::move(patch)){} void onRegister(SceneManager& mgr) override; math::AABB worldBounds() const override { return m_patch.bounds; } private: terrain::TerrainPatch m_patch;};

class SceneManager { public: explicit SceneManager(graphics::IVideoDriver& driver) : m_driver(driver) {} std::shared_ptr<SceneNode> root() const { return m_root; } std::shared_ptr<CameraSceneNode> createCamera(); std::shared_ptr<MeshSceneNode> createMeshNode(const std::shared_ptr<IMesh>& mesh); std::shared_ptr<LightSceneNode> createLightNode(const std::shared_ptr<Light>& light); std::shared_ptr<TerrainSceneNode> createTerrainNode(const terrain::TerrainPatch& patch); bool spawnModel(const std::string& assetPath, const NPlacementOptions& options, NSpawnReport* outReport = nullptr); bool spawnModel(const nova::asset::NModelData& modelData, const NPlacementOptions& options, NSpawnReport* outReport = nullptr); void setActiveCamera(const std::shared_ptr<CameraSceneNode>& camera) { m_camera = camera; } std::shared_ptr<CameraSceneNode> activeCamera() const { return m_camera; } void update(float dt); void gather(); void registerVisible(const std::shared_ptr<IMeshBuffer>& buffer, const math::Matrix4& world, RenderMobility mobility=RenderMobility::Dynamic, std::size_t subMeshIndex=0); void registerDirectional(const DirectionalLight& light); void registerPoint(const PointLight& light, const math::Vector3& pos); const std::vector<RenderItem>& opaqueQueue() const { return m_opaqueQueue; } const std::vector<RenderItem>& transparentQueue() const { return m_transparentQueue; } const LightingContext& lighting() const { return m_lighting; } void rebuildSpatial(); private: graphics::IVideoDriver& m_driver; std::shared_ptr<SceneNode> m_root=std::make_shared<SceneNode>(); std::shared_ptr<CameraSceneNode> m_camera; std::vector<RenderItem> m_staticOpaqueQueue; std::vector<RenderItem> m_dynamicOpaqueQueue; std::vector<RenderItem> m_opaqueQueue; std::vector<RenderItem> m_staticTransparentQueue; std::vector<RenderItem> m_dynamicTransparentQueue; std::vector<RenderItem> m_transparentQueue; LightingContext m_lighting; std::unique_ptr<spatial::SpatialPartition> m_staticPartition; std::unique_ptr<spatial::SpatialPartition> m_dynamicPartition; std::size_t m_spatialId=1;};

std::shared_ptr<IMesh> createSimpleTriangleMesh();
std::shared_ptr<IMesh> createMeshFromModelAsset(const assets::ModelAsset& model);

} // namespace
