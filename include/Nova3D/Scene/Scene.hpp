#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Core/Math/Math.hpp>
#include <Nova3D/Scene/IMesh.hpp>
#include <Nova3D/Graphics/IRenderer.hpp>

namespace nova3d::scene {

class SceneManager;

struct RenderItem {
    std::shared_ptr<IMeshBuffer> meshBuffer;
    math::Matrix4 world;
};

class ISceneNode : public std::enable_shared_from_this<ISceneNode> {
public:
    virtual ~ISceneNode() = default;
    virtual void onUpdate(float dt) = 0;
    virtual void onRegister(SceneManager& mgr) = 0;
    virtual math::AABB worldBounds() const = 0;
    math::Transform& transform() { return m_transform; }
    const math::Transform& transform() const { return m_transform; }
    void setVisible(bool visible) { m_visible = visible; }
    bool visible() const { return m_visible; }
    void addChild(const std::shared_ptr<ISceneNode>& child);
    const std::vector<std::shared_ptr<ISceneNode>>& children() const { return m_children; }
    std::shared_ptr<ISceneNode> parent() const { return m_parent.lock(); }
    const math::Matrix4& worldMatrix() const { return m_world; }
    void updateWorldRecursive(const math::Matrix4& parentWorld);
protected:
    math::Transform m_transform{};
    math::Matrix4 m_world = math::Matrix4::identity();
    bool m_visible = true;
    std::weak_ptr<ISceneNode> m_parent;
    std::vector<std::shared_ptr<ISceneNode>> m_children;
};

class SceneNode : public ISceneNode {
public:
    void onUpdate(float) override {}
    void onRegister(SceneManager&) override {}
    math::AABB worldBounds() const override { return {}; }
};

class CameraSceneNode : public SceneNode {
public:
    void setPerspective(float fovDeg, float aspect, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    void lookAt(const math::Vector3& target, const math::Vector3& up = {0,1,0});
    const math::Matrix4& viewMatrix() const { return m_view; }
    const math::Matrix4& projectionMatrix() const { return m_proj; }
    math::Frustum frustum() const { return math::Frustum::fromViewProjection(m_proj * m_view); }
private:
    math::Matrix4 m_view = math::Matrix4::identity();
    math::Matrix4 m_proj = math::Matrix4::identity();
};

class MeshSceneNode : public SceneNode {
public:
    explicit MeshSceneNode(std::shared_ptr<IMesh> mesh) : m_mesh(std::move(mesh)) {}
    void onRegister(SceneManager& mgr) override;
    math::AABB worldBounds() const override;
private:
    std::shared_ptr<IMesh> m_mesh;
};

class SceneManager {
public:
    explicit SceneManager(graphics::IVideoDriver& driver) : m_driver(driver) {}
    std::shared_ptr<SceneNode> root() const { return m_root; }
    std::shared_ptr<CameraSceneNode> createCamera();
    std::shared_ptr<MeshSceneNode> createMeshNode(const std::shared_ptr<IMesh>& mesh);
    void setActiveCamera(const std::shared_ptr<CameraSceneNode>& camera) { m_camera = camera; }
    void update(float dt);
    void render();
    void registerVisible(const std::shared_ptr<IMeshBuffer>& buffer, const math::Matrix4& world);
private:
    graphics::IVideoDriver& m_driver;
    std::shared_ptr<SceneNode> m_root = std::make_shared<SceneNode>();
    std::shared_ptr<CameraSceneNode> m_camera;
    std::vector<RenderItem> m_queue;
};

std::shared_ptr<IMesh> createSimpleTriangleMesh();

} // namespace
