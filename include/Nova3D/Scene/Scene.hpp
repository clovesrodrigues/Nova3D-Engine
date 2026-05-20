#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Core/Math/Math.hpp>
#include <Nova3D/Scene/IMesh.hpp>
#include <Nova3D/Graphics/IRenderer.hpp>

namespace nova3d::graphics { class ForwardRenderer; }
namespace nova3d::scene {

struct Light { math::Vector3 color{1,1,1}; float intensity=1.0F; virtual ~Light()=default; };
struct DirectionalLight final : Light { math::Vector3 direction{0,-1,0}; };
struct PointLight final : Light { float range=10.0F; math::Vector3 attenuation{1.0F,0.09F,0.032F}; };
struct SpotLight final : Light { math::Vector3 direction{0,-1,0}; float range=10.0F; float innerCone=0.7F; float outerCone=0.9F; };

struct RenderItem { std::shared_ptr<IMeshBuffer> meshBuffer; math::Matrix4 world;};
struct LightingContext { std::vector<DirectionalLight> directionalLights; std::vector<PointLight> pointLights; };

class SceneManager;
class ISceneNode : public std::enable_shared_from_this<ISceneNode> { public: virtual ~ISceneNode() = default; virtual void onUpdate(float dt) = 0; virtual void onRegister(SceneManager& mgr) = 0; virtual math::AABB worldBounds() const = 0; math::Transform& transform() { return m_transform; } const math::Transform& transform() const { return m_transform; } void setVisible(bool visible) { m_visible = visible; } bool visible() const { return m_visible; } void addChild(const std::shared_ptr<ISceneNode>& child); const std::vector<std::shared_ptr<ISceneNode>>& children() const { return m_children; } const math::Matrix4& worldMatrix() const { return m_world; } void updateWorldRecursive(const math::Matrix4& parentWorld); protected: math::Transform m_transform{}; math::Matrix4 m_world=math::Matrix4::identity(); bool m_visible=true; std::weak_ptr<ISceneNode> m_parent; std::vector<std::shared_ptr<ISceneNode>> m_children;};
class SceneNode : public ISceneNode { public: void onUpdate(float) override {} void onRegister(SceneManager&) override {} math::AABB worldBounds() const override { return {}; } };
class CameraSceneNode : public SceneNode { public: void setPerspective(float fovDeg, float aspect, float nearPlane, float farPlane); void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane); void lookAt(const math::Vector3& target, const math::Vector3& up = {0,1,0}); void setViewport(const graphics::Viewport& viewport){m_viewport=viewport;} const graphics::Viewport& viewport() const { return m_viewport; } void setRenderTarget(const std::shared_ptr<graphics::IRenderTarget>& target){m_target=target;} std::shared_ptr<graphics::IRenderTarget> renderTarget() const { return m_target; } const math::Matrix4& viewMatrix() const { return m_view; } const math::Matrix4& projectionMatrix() const { return m_proj; } math::Matrix4 viewProjectionMatrix() const { return m_proj*m_view; } math::Frustum frustum() const { return math::Frustum::fromViewProjection(viewProjectionMatrix()); } private: math::Matrix4 m_view=math::Matrix4::identity(); math::Matrix4 m_proj=math::Matrix4::identity(); graphics::Viewport m_viewport{0,0,1280,720}; std::shared_ptr<graphics::IRenderTarget> m_target; };
class MeshSceneNode : public SceneNode { public: explicit MeshSceneNode(std::shared_ptr<IMesh> mesh) : m_mesh(std::move(mesh)) {} void onRegister(SceneManager& mgr) override; math::AABB worldBounds() const override; private: std::shared_ptr<IMesh> m_mesh;};
class LightSceneNode : public SceneNode { public: explicit LightSceneNode(std::shared_ptr<Light> light):m_light(std::move(light)){} void onRegister(SceneManager& mgr) override; math::AABB worldBounds() const override; private: std::shared_ptr<Light> m_light;};

class SceneManager { public: explicit SceneManager(graphics::IVideoDriver& driver) : m_driver(driver) {} std::shared_ptr<SceneNode> root() const { return m_root; } std::shared_ptr<CameraSceneNode> createCamera(); std::shared_ptr<MeshSceneNode> createMeshNode(const std::shared_ptr<IMesh>& mesh); std::shared_ptr<LightSceneNode> createLightNode(const std::shared_ptr<Light>& light); void setActiveCamera(const std::shared_ptr<CameraSceneNode>& camera) { m_camera = camera; } std::shared_ptr<CameraSceneNode> activeCamera() const { return m_camera; } void update(float dt); void gather(); void registerVisible(const std::shared_ptr<IMeshBuffer>& buffer, const math::Matrix4& world); void registerDirectional(const DirectionalLight& light); void registerPoint(const PointLight& light, const math::Vector3& pos); const std::vector<RenderItem>& opaqueQueue() const { return m_opaqueQueue; } const std::vector<RenderItem>& transparentQueue() const { return m_transparentQueue; } const LightingContext& lighting() const { return m_lighting; } private: graphics::IVideoDriver& m_driver; std::shared_ptr<SceneNode> m_root=std::make_shared<SceneNode>(); std::shared_ptr<CameraSceneNode> m_camera; std::vector<RenderItem> m_opaqueQueue; std::vector<RenderItem> m_transparentQueue; LightingContext m_lighting;};

std::shared_ptr<IMesh> createSimpleTriangleMesh();

} // namespace
