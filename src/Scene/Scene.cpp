#include <Nova3D/Scene/Scene.hpp>

namespace nova3d::scene {

void ISceneNode::addChild(const std::shared_ptr<ISceneNode>& child) { child->m_parent = shared_from_this(); m_children.push_back(child); }
void ISceneNode::updateWorldRecursive(const math::Matrix4& parentWorld) {
    m_world = m_transform.worldMatrix(parentWorld);
    for (auto& c : m_children) c->updateWorldRecursive(m_world);
}

void CameraSceneNode::setPerspective(float fovDeg, float aspect, float nearPlane, float farPlane) { m_proj = math::Matrix4::perspective(math::toRadians(fovDeg), aspect, nearPlane, farPlane); }
void CameraSceneNode::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) { m_proj = math::Matrix4::orthographic(left,right,bottom,top,nearPlane,farPlane); }
void CameraSceneNode::lookAt(const math::Vector3& target, const math::Vector3& up) { m_view = math::Matrix4::lookAt(transform().position, target, up); }

void MeshSceneNode::onRegister(SceneManager& mgr) {
    if (!m_mesh) return;
    for (std::size_t i=0;i<m_mesh->meshBufferCount();++i) mgr.registerVisible(m_mesh->meshBuffer(i), worldMatrix());
}

math::AABB MeshSceneNode::worldBounds() const { return m_mesh ? m_mesh->boundingBox() : math::AABB{}; }

std::shared_ptr<CameraSceneNode> SceneManager::createCamera(){ auto c=std::make_shared<CameraSceneNode>(); m_root->addChild(c); return c; }
std::shared_ptr<MeshSceneNode> SceneManager::createMeshNode(const std::shared_ptr<IMesh>& mesh){ auto n=std::make_shared<MeshSceneNode>(mesh); m_root->addChild(n); return n; }

void SceneManager::update(float dt) {
    m_root->updateWorldRecursive(math::Matrix4::identity());
    std::vector<std::shared_ptr<ISceneNode>> stack{m_root};
    while(!stack.empty()) { auto n=stack.back(); stack.pop_back(); n->onUpdate(dt); for (auto& c : n->children()) stack.push_back(c); }
}
void SceneManager::registerVisible(const std::shared_ptr<IMeshBuffer>& buffer, const math::Matrix4& world) { m_queue.push_back({buffer, world}); }
void SceneManager::render() {
    if (!m_camera) return;
    m_queue.clear();
    const auto fr = m_camera->frustum();
    std::vector<std::shared_ptr<ISceneNode>> stack{m_root};
    while(!stack.empty()) { auto n=stack.back(); stack.pop_back(); if(n->visible()) { if (auto b=n->worldBounds(); fr.intersects(b)) n->onRegister(*this); for (auto& c : n->children()) stack.push_back(c); } }
    for (auto& item : m_queue) {
        auto vb = m_driver.createVertexBuffer(); auto ib = m_driver.createIndexBuffer();
        graphics::VertexLayout layout{}; layout.stride=sizeof(Vertex); layout.attributeCount=2; layout.attributes[0]={graphics::VertexSemantic::Position,0,3,offsetof(Vertex, position)}; layout.attributes[1]={graphics::VertexSemantic::Color0,1,3,offsetof(Vertex, color)}; vb->upload(item.meshBuffer->vertices().data(), item.meshBuffer->vertices().size()*sizeof(Vertex), layout);
        ib->upload(item.meshBuffer->indices().data(), item.meshBuffer->indices().size());
        if (auto material = item.meshBuffer->material()) { m_driver.setRenderState(material->state()); material->bind(); }
        vb->bind(); ib->bind();
        m_driver.drawIndexed(item.meshBuffer->primitiveType(), ib->indexCount());
        if (auto material = item.meshBuffer->material()) material->unbind();
    }
}

class MeshBuffer final : public IMeshBuffer { public:
    std::vector<Vertex> v; std::vector<std::uint32_t> i; math::AABB box; graphics::PrimitiveType type=graphics::PrimitiveType::Triangles;
    const std::vector<Vertex>& vertices() const override { return v; }
    const std::vector<std::uint32_t>& indices() const override { return i; }
    graphics::PrimitiveType primitiveType() const override { return type; }
    const math::AABB& boundingBox() const override { return box; }
    std::shared_ptr<graphics::IMaterial> material() const override { return {}; }
};
class Mesh final : public IMesh { public: std::vector<std::shared_ptr<IMeshBuffer>> bufs; math::AABB box;
    std::size_t meshBufferCount() const override { return bufs.size(); }
    std::shared_ptr<IMeshBuffer> meshBuffer(std::size_t index) const override { return bufs.at(index); }
    const math::AABB& boundingBox() const override { return box; }
};

std::shared_ptr<IMesh> createSimpleTriangleMesh(){ auto mb=std::make_shared<MeshBuffer>(); mb->v={{{0,0.5F,0},{1,0,0}},{{-0.5F,-0.5F,0},{0,1,0}},{{0.5F,-0.5F,0},{0,0,1}}}; mb->i={0,1,2}; mb->box={{-0.5F,-0.5F,0},{0.5F,0.5F,0}}; auto m=std::make_shared<Mesh>(); m->bufs.push_back(mb); m->box=mb->box; return m; }

} // namespace
