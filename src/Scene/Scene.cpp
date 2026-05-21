#include <Nova3D/Assets/Importers/MeshImporter.hpp>
#include <Nova3D/Scene/Scene.hpp>

namespace nova3d::scene {
void ISceneNode::addChild(const std::shared_ptr<ISceneNode>& child){ child->m_parent=shared_from_this(); m_children.push_back(child);} 
void ISceneNode::updateWorldRecursive(const math::Matrix4& parentWorld){ m_world=m_transform.worldMatrix(parentWorld); for(auto& c:m_children) c->updateWorldRecursive(m_world);} 
void CameraSceneNode::setPerspective(float fovDeg,float aspect,float nearPlane,float farPlane){ m_proj=math::Matrix4::perspective(math::toRadians(fovDeg),aspect,nearPlane,farPlane);} 
void CameraSceneNode::setOrthographic(float left,float right,float bottom,float top,float nearPlane,float farPlane){ m_proj=math::Matrix4::orthographic(left,right,bottom,top,nearPlane,farPlane);} 
void CameraSceneNode::lookAt(const math::Vector3& target,const math::Vector3& up){ m_view=math::Matrix4::lookAt(transform().position,target,up);} 

void MeshSceneNode::onRegister(SceneManager& mgr){ if(!m_mesh) return; for(size_t i=0;i<m_mesh->meshBufferCount();++i) mgr.registerVisible(m_mesh->meshBuffer(i), worldMatrix(), RenderMobility::Dynamic, i); }
math::AABB MeshSceneNode::worldBounds() const { return m_mesh?m_mesh->boundingBox():math::AABB{}; }
void LightSceneNode::onRegister(SceneManager& mgr){
    if (auto directional = std::dynamic_pointer_cast<DirectionalLight>(m_light)) { directional->direction={0,-1,0}; mgr.registerDirectional(*directional); }
    if (auto point = std::dynamic_pointer_cast<PointLight>(m_light)) { mgr.registerPoint(*point, transform().position); }
}
math::AABB LightSceneNode::worldBounds() const { return {{-0.2F,-0.2F,-0.2F},{0.2F,0.2F,0.2F}}; }
void TerrainSceneNode::onRegister(SceneManager&){}

std::shared_ptr<CameraSceneNode> SceneManager::createCamera(){ auto c=std::make_shared<CameraSceneNode>(); m_root->addChild(c); return c; }
std::shared_ptr<MeshSceneNode> SceneManager::createMeshNode(const std::shared_ptr<IMesh>& mesh){ auto n=std::make_shared<MeshSceneNode>(mesh); m_root->addChild(n); return n; }
std::shared_ptr<LightSceneNode> SceneManager::createLightNode(const std::shared_ptr<Light>& light){ auto n=std::make_shared<LightSceneNode>(light); m_root->addChild(n); return n; }
std::shared_ptr<TerrainSceneNode> SceneManager::createTerrainNode(const terrain::TerrainPatch& patch){ auto n=std::make_shared<TerrainSceneNode>(patch); m_root->addChild(n); return n; }
void SceneManager::update(float dt){ m_root->updateWorldRecursive(math::Matrix4::identity()); std::vector<std::shared_ptr<ISceneNode>> stack{m_root}; while(!stack.empty()){ auto n=stack.back(); stack.pop_back(); n->onUpdate(dt); for(auto& c:n->children()) stack.push_back(c);} }
void SceneManager::registerVisible(const std::shared_ptr<IMeshBuffer>& buffer,const math::Matrix4& world, RenderMobility mobility, std::size_t subMeshIndex){ RenderItem item{buffer,world,subMeshIndex,mobility}; if (auto m=buffer->material(); m && m->isTransparent()) { if(mobility==RenderMobility::Static) m_staticTransparentQueue.push_back(item); else m_dynamicTransparentQueue.push_back(item);} else { if(mobility==RenderMobility::Static) m_staticOpaqueQueue.push_back(item); else m_dynamicOpaqueQueue.push_back(item);} }
void SceneManager::registerDirectional(const DirectionalLight& light){ m_lighting.directionalLights.push_back(light);} void SceneManager::registerPoint(const PointLight& light,const math::Vector3&){ m_lighting.pointLights.push_back(light); }
void SceneManager::rebuildSpatial(){ if(!m_staticPartition) m_staticPartition=std::make_unique<spatial::OctreePartition>(math::AABB{{-2048,-2048,-2048},{2048,2048,2048}}); if(!m_dynamicPartition) m_dynamicPartition=std::make_unique<spatial::BVHPartition>(); m_staticPartition=std::make_unique<spatial::OctreePartition>(math::AABB{{-2048,-2048,-2048},{2048,2048,2048}}); m_dynamicPartition=std::make_unique<spatial::BVHPartition>(); std::vector<std::shared_ptr<ISceneNode>> stack{m_root}; while(!stack.empty()){ auto n=stack.back(); stack.pop_back(); if(n.get()!=m_root.get()){ spatial::SpatialNode sn{m_spatialId++,n->worldBounds(),n.get(),true}; m_dynamicPartition->insert(sn);} for(auto& c:n->children()) stack.push_back(c);} }
void SceneManager::gather(){ if(!m_camera) return; rebuildSpatial(); m_opaqueQueue.clear(); m_transparentQueue.clear(); m_staticOpaqueQueue.clear(); m_dynamicOpaqueQueue.clear(); m_staticTransparentQueue.clear(); m_dynamicTransparentQueue.clear(); m_lighting.directionalLights.clear(); m_lighting.pointLights.clear(); auto fr=m_camera->frustum(); std::vector<std::shared_ptr<ISceneNode>> stack{m_root}; while(!stack.empty()){ auto n=stack.back(); stack.pop_back(); if(!n->visible()) continue; const auto wb=n->worldBounds(); if(fr.intersects(wb) || fr.intersects(math::Sphere{wb.center(),wb.radius()})) n->onRegister(*this); for(auto& c:n->children()) stack.push_back(c);} m_opaqueQueue.insert(m_opaqueQueue.end(),m_staticOpaqueQueue.begin(),m_staticOpaqueQueue.end()); m_opaqueQueue.insert(m_opaqueQueue.end(),m_dynamicOpaqueQueue.begin(),m_dynamicOpaqueQueue.end()); m_transparentQueue.insert(m_transparentQueue.end(),m_staticTransparentQueue.begin(),m_staticTransparentQueue.end()); m_transparentQueue.insert(m_transparentQueue.end(),m_dynamicTransparentQueue.begin(),m_dynamicTransparentQueue.end()); }

class MeshBuffer final : public IMeshBuffer { public: std::vector<Vertex> v; std::vector<std::uint32_t> i; math::AABB box; graphics::PrimitiveType type=graphics::PrimitiveType::Triangles; std::shared_ptr<graphics::IMaterial> mat; const std::vector<Vertex>& vertices() const override {return v;} const std::vector<std::uint32_t>& indices() const override {return i;} graphics::PrimitiveType primitiveType() const override {return type;} const math::AABB& boundingBox() const override {return box;} std::shared_ptr<graphics::IMaterial> material() const override {return mat;} };
class Mesh final : public IMesh { public: std::vector<std::shared_ptr<IMeshBuffer>> bufs; math::AABB box; std::size_t meshBufferCount() const override {return bufs.size();} std::shared_ptr<IMeshBuffer> meshBuffer(std::size_t idx) const override {return bufs.at(idx);} const math::AABB& boundingBox() const override {return box;} };
std::shared_ptr<IMesh> createSimpleTriangleMesh(){ auto mb=std::make_shared<MeshBuffer>(); mb->v={{{0,0.6F,0},{0,0,1},{0.5F,1.0F},{1,0,0,1}},{{-0.6F,-0.6F,0},{0,0,1},{0,0},{0,1,0,1}},{{0.6F,-0.6F,0},{0,0,1},{1,0},{0,0,1,1}}}; mb->i={0,1,2}; mb->box={{-0.6F,-0.6F,0},{0.6F,0.6F,0}}; auto m=std::make_shared<Mesh>(); m->bufs.push_back(mb); m->box=mb->box; return m; }
std::shared_ptr<IMesh> createMeshFromModelAsset(const assets::ModelAsset& model){ auto mb=std::make_shared<MeshBuffer>(); for(auto &v:model.mesh.vertices) mb->v.push_back({v.position,v.normal,v.uv,v.color}); mb->i=model.mesh.indices; mb->box=model.mesh.bounds; auto m=std::make_shared<Mesh>(); m->bufs.push_back(mb); m->box=model.mesh.bounds; return m; }

}
