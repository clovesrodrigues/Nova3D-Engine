#include <Nova3D/Assets/Importers/MeshImporter.hpp>
#include <Nova3D/Core/Logger.hpp>
#include <Nova3D/Scene/Scene.hpp>

#include <limits>

#include <algorithm>
#include <chrono>

namespace {
using namespace nova3d;

bool validAABB(const math::AABB& b){
    return b.min.x<=b.max.x && b.min.y<=b.max.y && b.min.z<=b.max.z;
}

math::AABB mergeAABB(const math::AABB& a, const math::AABB& b){
    return {{std::min(a.min.x,b.min.x),std::min(a.min.y,b.min.y),std::min(a.min.z,b.min.z)},
            {std::max(a.max.x,b.max.x),std::max(a.max.y,b.max.y),std::max(a.max.z,b.max.z)}};
}

class MeshBuffer final : public scene::IMeshBuffer { public: std::vector<scene::Vertex> v; std::vector<std::uint32_t> i; math::AABB box; graphics::PrimitiveType type=graphics::PrimitiveType::Triangles; std::shared_ptr<graphics::IMaterial> mat; const std::vector<scene::Vertex>& vertices() const override {return v;} const std::vector<std::uint32_t>& indices() const override {return i;} graphics::PrimitiveType primitiveType() const override {return type;} const math::AABB& boundingBox() const override {return box;} std::shared_ptr<graphics::IMaterial> material() const override {return mat;} };
class Mesh final : public scene::IMesh { public: std::vector<std::shared_ptr<scene::IMeshBuffer>> bufs; math::AABB box; std::size_t meshBufferCount() const override {return bufs.size();} std::shared_ptr<scene::IMeshBuffer> meshBuffer(std::size_t idx) const override {return bufs.at(idx);} const math::AABB& boundingBox() const override {return box;} };

std::shared_ptr<scene::IMesh> createMeshFromImported(const nova::asset::NMesh& mesh){
    auto mb=std::make_shared<MeshBuffer>();
    for(auto &v:mesh.vertices) mb->v.push_back({v.position,v.normal,v.uv,v.color});
    mb->i=mesh.indices; mb->box=mesh.bounds;
    auto m=std::make_shared<Mesh>(); m->bufs.push_back(mb); m->box=mesh.bounds; return m;
}

math::Quaternion eulerToQuaternion(const math::Vector3& e){
    const float cy=std::cos(e.y*0.5F), sy=std::sin(e.y*0.5F);
    const float cp=std::cos(e.x*0.5F), sp=std::sin(e.x*0.5F);
    const float cr=std::cos(e.z*0.5F), sr=std::sin(e.z*0.5F);
    return {cr*cp*cy + sr*sp*sy, sr*cp*cy - cr*sp*sy, cr*sp*cy + sr*cp*sy, cr*cp*sy - sr*sp*cy};
}
}

namespace nova3d::scene {
void ISceneNode::addChild(const std::shared_ptr<ISceneNode>& child){ child->m_parent=shared_from_this(); m_children.push_back(child);} 
void ISceneNode::updateWorldRecursive(const math::Matrix4& parentWorld){ m_world=m_transform.worldMatrix(parentWorld); for(auto& c:m_children) c->updateWorldRecursive(m_world);} 
void CameraSceneNode::setPerspective(float fovDeg,float aspect,float nearPlane,float farPlane){ m_proj=math::Matrix4::perspective(math::toRadians(fovDeg),aspect,nearPlane,farPlane);} 
void CameraSceneNode::setOrthographic(float left,float right,float bottom,float top,float nearPlane,float farPlane){ m_proj=math::Matrix4::orthographic(left,right,bottom,top,nearPlane,farPlane);} 
void CameraSceneNode::lookAt(const math::Vector3& target,const math::Vector3& up){ m_view=math::Matrix4::lookAt(transform().position,target,up);} 

void CameraTargetSceneNode::onUpdate(float){
    if(!m_followLinkedNode) return;
    if(auto n=m_linkedNode.lock()){
        transform().position = n->transform().position + m_offset;
        transform().markDirty();
    }
}

void CameraSceneNode::onUpdate(float){
    if(auto t=m_targetNode.lock()){
        if(m_lockCameraAndTarget){
            t->transform().position = transform().position + m_lockOffset;
            t->transform().markDirty();
        }
        lookAt(t->transform().position);
    }
}

math::Vector3 computePivotOffset(const math::AABB& bounds, NPivotMode mode, const math::Vector3& customPivotOffset){
    const auto c=bounds.center();
    switch(mode){
        case NPivotMode::MeshCenter: return c;
        case NPivotMode::BottomCenter: return {c.x,bounds.min.y,c.z};
        case NPivotMode::TopCenter: return {c.x,bounds.max.y,c.z};
        case NPivotMode::Custom: return customPivotOffset;
    }
    return c;
}

void MeshSceneNode::onRegister(SceneManager& mgr){ if(!m_mesh) return; for(size_t i=0;i<m_mesh->meshBufferCount();++i) mgr.registerVisible(m_mesh->meshBuffer(i), worldMatrix(), RenderMobility::Dynamic, i); }
math::AABB MeshSceneNode::worldBounds() const { return m_mesh?m_mesh->boundingBox():math::AABB{}; }
void LightSceneNode::onRegister(SceneManager& mgr){
    if (auto directional = std::dynamic_pointer_cast<DirectionalLight>(m_light)) { directional->direction={0,-1,0}; mgr.registerDirectional(*directional); }
    if (auto point = std::dynamic_pointer_cast<PointLight>(m_light)) { mgr.registerPoint(*point, transform().position); }
}
math::AABB LightSceneNode::worldBounds() const { return {{-0.2F,-0.2F,-0.2F},{0.2F,0.2F,0.2F}}; }
void TerrainSceneNode::onRegister(SceneManager&){}

std::shared_ptr<CameraSceneNode> SceneManager::createCamera(){ auto c=std::make_shared<CameraSceneNode>(); m_root->addChild(c); return c; }
std::shared_ptr<CameraTargetSceneNode> SceneManager::createCameraTargetNode(const math::Vector3& initialOffset){ auto n=std::make_shared<CameraTargetSceneNode>(initialOffset); m_root->addChild(n); return n; }
std::shared_ptr<MeshSceneNode> SceneManager::createMeshNode(const std::shared_ptr<IMesh>& mesh){ auto n=std::make_shared<MeshSceneNode>(mesh); m_root->addChild(n); return n; }
std::shared_ptr<CharacterSceneNode> SceneManager::createCharacterNode(float radius,float height){ auto n=std::make_shared<CharacterSceneNode>(radius,height); m_root->addChild(n); return n; }
std::shared_ptr<BillboardSceneNode> SceneManager::createBillboardNode(const math::Vector2& size){ auto n=std::make_shared<BillboardSceneNode>(size); m_root->addChild(n); return n; }
std::shared_ptr<LightSceneNode> SceneManager::createLightNode(const std::shared_ptr<Light>& light){ auto n=std::make_shared<LightSceneNode>(light); m_root->addChild(n); return n; }
std::shared_ptr<TerrainSceneNode> SceneManager::createTerrainNode(const terrain::TerrainPatch& patch){ auto n=std::make_shared<TerrainSceneNode>(patch); m_root->addChild(n); return n; }

bool SceneManager::spawnModel(const std::string& assetPath, const NPlacementOptions& options, NSpawnReport* outReport){
    nova::asset::NModelImporter importer;
    auto imported=importer.importFromFile(assetPath);
    NSpawnReport local{}; local.assetPath=assetPath; local.warnings=imported.report.warnings; local.errors=imported.report.errors;
    if(!imported.report.success){ local.success=false; if(outReport)*outReport=local; return false; }
    const bool ok=spawnModel(imported.data, options, &local); local.assetPath=assetPath;
    if(outReport)*outReport=local; return ok;
}

bool SceneManager::spawnModel(const nova::asset::NModelData& modelData, const NPlacementOptions& options, NSpawnReport* outReport){
    NSpawnReport report{}; report.createdNodeCount=0;
    if(modelData.meshes.empty()){ report.errors.emplace_back("Model has no meshes."); if(outReport)*outReport=report; return false; }
    math::AABB bounds=modelData.meshes.front().bounds;
    for(size_t i=1;i<modelData.meshes.size();++i) bounds=mergeAABB(bounds, modelData.meshes[i].bounds);
    if(!validAABB(bounds)){ report.errors.emplace_back("Invalid model bounds."); if(outReport)*outReport=report; return false; }
    report.modelBounds=bounds;
    const auto pivot=computePivotOffset(bounds, options.pivotMode, options.customPivotOffset);
    report.computedPivotWorld=options.initialPosition;
    math::Vector3 finalPos = options.initialPosition - pivot;
    if(options.alignToGround){ finalPos.y -= bounds.min.y; }
    for(const auto& importedMesh:modelData.meshes){
        auto node=createMeshNode(createMeshFromImported(importedMesh));
        node->transform().position=finalPos;
        node->transform().scale=options.initialScale;
        node->transform().rotation=eulerToQuaternion(options.initialRotationEuler);
        if(options.useLookAt){
            const auto f=(options.lookAtTarget-options.initialPosition);
            const auto up=options.upVector.length()>1e-4F?options.upVector:math::Vector3{0,1,0};
            if(f.length()>1e-4F){
                auto view=math::Matrix4::lookAt(options.initialPosition, options.lookAtTarget, up);
                // inverse rotation from view basis
                math::Matrix4 w=math::Matrix4::identity();
                for(int r=0;r<3;++r)for(int c=0;c<3;++c) w.at(r,c)=view.at(c,r);
                const float tr=w.at(0,0)+w.at(1,1)+w.at(2,2);
                math::Quaternion q{};
                if(tr>0){ float s=std::sqrt(tr+1.0F)*2; q.w=0.25F*s; q.x=(w.at(2,1)-w.at(1,2))/s; q.y=(w.at(0,2)-w.at(2,0))/s; q.z=(w.at(1,0)-w.at(0,1))/s; }
                node->transform().rotation=q;
            } else { report.warnings.emplace_back("Invalid lookAt target direction; keeping initial rotation."); }
        }
        node->transform().markDirty();
        ++report.createdNodeCount;
    }
    report.success=true; if(outReport)*outReport=report;
    return true;
}

void SceneManager::registerVisible(const std::shared_ptr<IMeshBuffer>& buffer,const math::Matrix4& world, RenderMobility mobility, std::size_t subMeshIndex){ RenderItem item{buffer,world,subMeshIndex,mobility}; if (auto m=buffer->material(); m && m->isTransparent()) { if(mobility==RenderMobility::Static) m_staticTransparentQueue.push_back(item); else m_dynamicTransparentQueue.push_back(item);} else { if(mobility==RenderMobility::Static) m_staticOpaqueQueue.push_back(item); else m_dynamicOpaqueQueue.push_back(item);} }
void SceneManager::registerDirectional(const DirectionalLight& light){ m_lighting.directionalLights.push_back(light);} void SceneManager::registerPoint(const PointLight& light,const math::Vector3&){ m_lighting.pointLights.push_back(light); }
void SceneManager::rebuildSpatial(){ if(!m_staticPartition) m_staticPartition=std::make_unique<spatial::OctreePartition>(math::AABB{{-2048,-2048,-2048},{2048,2048,2048}}); if(!m_dynamicPartition) m_dynamicPartition=std::make_unique<spatial::BVHPartition>(); m_staticPartition=std::make_unique<spatial::OctreePartition>(math::AABB{{-2048,-2048,-2048},{2048,2048,2048}}); m_dynamicPartition=std::make_unique<spatial::BVHPartition>(); std::vector<std::shared_ptr<ISceneNode>> stack{m_root}; while(!stack.empty()){ auto n=stack.back(); stack.pop_back(); if(n.get()!=m_root.get()){ spatial::SpatialNode sn{m_spatialId++,n->worldBounds(),n.get(),true}; m_dynamicPartition->insert(sn);} for(auto& c:n->children()) stack.push_back(c);} }
void SceneManager::gather(){ if(!m_camera) return; rebuildSpatial(); m_opaqueQueue.clear(); m_transparentQueue.clear(); m_staticOpaqueQueue.clear(); m_dynamicOpaqueQueue.clear(); m_staticTransparentQueue.clear(); m_dynamicTransparentQueue.clear(); m_lighting.directionalLights.clear(); m_lighting.pointLights.clear(); auto fr=m_camera->frustum(); std::vector<std::shared_ptr<ISceneNode>> stack{m_root}; while(!stack.empty()){ auto n=stack.back(); stack.pop_back(); if(!n->visible()) continue; const auto wb=n->worldBounds(); if(fr.intersects(wb) || fr.intersects(math::Sphere{wb.center(),wb.radius()})) n->onRegister(*this); for(auto& c:n->children()) stack.push_back(c);} m_opaqueQueue.insert(m_opaqueQueue.end(),m_staticOpaqueQueue.begin(),m_staticOpaqueQueue.end()); m_opaqueQueue.insert(m_opaqueQueue.end(),m_dynamicOpaqueQueue.begin(),m_dynamicOpaqueQueue.end()); m_transparentQueue.insert(m_transparentQueue.end(),m_staticTransparentQueue.begin(),m_staticTransparentQueue.end()); m_transparentQueue.insert(m_transparentQueue.end(),m_dynamicTransparentQueue.begin(),m_dynamicTransparentQueue.end()); }

std::shared_ptr<IMesh> createSimpleTriangleMesh(){ auto mb=std::make_shared<MeshBuffer>(); mb->v={{{0,0.6F,0},{0,0,1},{0.5F,1.0F},{1,0,0,1}},{{-0.6F,-0.6F,0},{0,0,1},{0,0},{0,1,0,1}},{{0.6F,-0.6F,0},{0,0,1},{1,0},{0,0,1,1}}}; mb->i={0,1,2}; mb->box={{-0.6F,-0.6F,0},{0.6F,0.6F,0}}; auto m=std::make_shared<Mesh>(); m->bufs.push_back(mb); m->box=mb->box; return m; }

void NTransformGizmo::setPosition(const math::Vector3& p){ if(!m_node) return; m_node->transform().position=p; m_node->transform().markDirty(); }
void NTransformGizmo::setRotation(const math::Quaternion& q){ if(!m_node) return; m_node->transform().rotation=q; m_node->transform().markDirty(); }
void NTransformGizmo::setScale(const math::Vector3& s){ if(!m_node) return; m_node->transform().scale=s; m_node->transform().markDirty(); }
void NTransformGizmo::translate(const math::Vector3& delta){ if(!m_node) return; m_node->transform().position=m_node->transform().position+delta; m_node->transform().markDirty(); }
void NTransformGizmo::rotate(const math::Quaternion& delta){ if(!m_node) return; m_node->transform().rotation=delta; m_node->transform().markDirty(); }
void NTransformGizmo::scaleBy(const math::Vector3& factor){ if(!m_node) return; auto& s=m_node->transform().scale; s={s.x*factor.x,s.y*factor.y,s.z*factor.z}; m_node->transform().markDirty(); }
}

namespace nova3d::scene {
std::shared_ptr<IMesh> createMeshFromModelAsset(const assets::ModelAsset& model){ auto mb=std::make_shared<MeshBuffer>(); for(auto &v:model.mesh.vertices) mb->v.push_back({v.position,v.normal,v.uv,v.color}); mb->i=model.mesh.indices; mb->box=model.mesh.bounds; auto m=std::make_shared<Mesh>(); m->bufs.push_back(mb); m->box=model.mesh.bounds; return m; }
}
namespace nova3d::scene {
SceneManager::SceneManager(graphics::IVideoDriver& driver):m_driver(driver){ m_metaSelector=std::make_shared<NMetaTriangleSelector>(); }
void SceneManager::setActiveCamera(const std::shared_ptr<CameraSceneNode>& camera){ m_camera=camera; m_collisionManager=std::make_shared<NSceneCollisionManager>(m_camera,m_metaSelector); }
void NSceneNodeAnimatorController::attach(const std::shared_ptr<ISceneNode>& node, const std::shared_ptr<ISceneNodeAnimator>& animator){ m_bindings.emplace_back(node,animator);} 
void NSceneNodeAnimatorController::detach(const std::shared_ptr<ISceneNode>& node, const std::shared_ptr<ISceneNodeAnimator>& animator){ m_bindings.erase(std::remove_if(m_bindings.begin(),m_bindings.end(),[&](auto& b){return b.first.lock()==node&&b.second==animator;}),m_bindings.end()); }
std::vector<std::shared_ptr<ISceneNodeAnimator>> NSceneNodeAnimatorController::list(const std::shared_ptr<ISceneNode>& node) const { std::vector<std::shared_ptr<ISceneNodeAnimator>> out; for(auto& b:m_bindings) if(b.first.lock()==node) out.push_back(b.second); return out; }
void NSceneNodeAnimatorController::update(float dt) const { auto sorted=m_bindings; std::sort(sorted.begin(),sorted.end(),[](auto& a, auto& b){ return a.second->priority()<b.second->priority();}); for(auto& b:sorted){ auto n=b.first.lock(); if(n && b.second && b.second->enabled()) b.second->animateNode(*n,dt);} }
void NAnimatorCameraFPS::animateNode(ISceneNode& node,float dt){ m_yaw += m_look.x*m_sensitivity*dt; m_pitch = std::clamp(m_pitch + m_look.y*m_sensitivity*dt,m_pitchMin,m_pitchMax); const float yaw=math::toRadians(m_yaw), pitch=math::toRadians(m_pitch); math::Vector3 f{std::cos(pitch)*std::sin(yaw), std::sin(pitch), -std::cos(pitch)*std::cos(yaw)}; auto r=f.cross({0,1,0}).normalized(); node.transform().position = node.transform().position + f*(m_move.x*m_speed*dt) + r*(m_move.y*m_speed*dt) + math::Vector3{0,1,0}*(m_move.z*m_speed*dt); node.transform().markDirty(); }
void NAnimatorCameraOrbit::animateNode(ISceneNode& node,float dt){ m_yaw+=m_orbit.x*dt; m_pitch=std::clamp(m_pitch+m_orbit.y*dt,-89.0F,89.0F); m_radius=std::max(0.5F,m_radius-m_dolly*dt); m_target=m_target+math::Vector3{m_pan.x*dt,m_pan.y*dt,0}; const float yr=math::toRadians(m_yaw), pr=math::toRadians(m_pitch); node.transform().position={m_target.x+std::cos(pr)*std::sin(yr)*m_radius,m_target.y+std::sin(pr)*m_radius,m_target.z-std::cos(pr)*std::cos(yr)*m_radius}; node.transform().markDirty(); if(auto* c=dynamic_cast<CameraSceneNode*>(&node)) c->lookAt(m_target); }
void NAnimatorCollisionResponse::animateNode(ISceneNode& node,float dt){ if(!m_collision) return; m_verticalVelocity-=m_gravity*dt; auto p=node.transform().position; p.y += m_verticalVelocity*dt; auto hit=m_collision->queryGround(p,100.0F); if(hit.hit && (p.y-hit.position.y)<m_stepOffset){ p.y=hit.position.y+m_stepOffset; m_verticalVelocity=0; } node.transform().position=p; node.transform().markDirty(); }
std::vector<Triangle> NMeshTriangleSelector::collectTriangles(const math::AABB&) const { std::vector<Triangle> out; if(!m_node||!m_node->mesh()||!m_node->pickingEnabled()) return out; auto m=m_node->mesh(); const auto world = m_node->worldMatrix(); for(size_t bi=0;bi<m->meshBufferCount();++bi){ auto b=m->meshBuffer(bi); auto& v=b->vertices(); auto& idx=b->indices(); for(size_t i=0;i+2<idx.size();i+=3){ auto p0=world.transformPoint(v[idx[i]].position); auto p1=world.transformPoint(v[idx[i+1]].position); auto p2=world.transformPoint(v[idx[i+2]].position); out.push_back({p0,p1,p2}); }} return out; }
std::vector<Triangle> NMeshTriangleSelector::collectTriangles(const math::Ray&, float) const { return collectTriangles(math::AABB{}); }
std::vector<Triangle> NMeshTriangleSelector::collectTriangles(const math::Frustum&) const { return collectTriangles(math::AABB{}); }
void NMetaTriangleSelector::addSelector(const std::shared_ptr<ITriangleSelector>& selector){ m_selectors.push_back(selector);} 
void NMetaTriangleSelector::removeSelector(const std::shared_ptr<ITriangleSelector>& selector){ m_selectors.erase(std::remove(m_selectors.begin(),m_selectors.end(),selector),m_selectors.end()); }
std::vector<Triangle> NMetaTriangleSelector::collectTriangles(const math::AABB& region) const { std::vector<Triangle> o; for(auto&s:m_selectors){auto t=s->collectTriangles(region);o.insert(o.end(),t.begin(),t.end());} return o; }
std::vector<Triangle> NMetaTriangleSelector::collectTriangles(const math::Ray& ray, float maxDistance) const { std::vector<Triangle> o; for(auto&s:m_selectors){auto t=s->collectTriangles(ray,maxDistance);o.insert(o.end(),t.begin(),t.end());} return o; }
std::vector<Triangle> NMetaTriangleSelector::collectTriangles(const math::Frustum& frustum) const { std::vector<Triangle> o; for(auto&s:m_selectors){auto t=s->collectTriangles(frustum);o.insert(o.end(),t.begin(),t.end());} return o; }
static bool rayTriangle(const math::Ray& ray,const Triangle& t,float& dist,math::Vector3& n){ const float eps=1e-6F; auto e1=t.b-t.a,e2=t.c-t.a; auto p=ray.direction.cross(e2); float det=e1.dot(p); if(std::abs(det)<eps) return false; float inv=1.0F/det; auto tv=ray.origin-t.a; float u=tv.dot(p)*inv; if(u<0||u>1) return false; auto q=tv.cross(e1); float v=ray.direction.dot(q)*inv; if(v<0||u+v>1) return false; dist=e2.dot(q)*inv; if(dist<0) return false; n=e1.cross(e2).normalized(); return true; }
SceneRayHit NSceneCollisionManager::rayPick(const math::Ray& ray,float maxDistance) const { SceneRayHit out{}; if(!m_selector) return out; auto t0=std::chrono::high_resolution_clock::now(); auto tris=m_selector->collectTriangles(ray,maxDistance); out.trianglesTested=tris.size(); float best=maxDistance; for(size_t i=0;i<tris.size();++i){ float d; math::Vector3 n; if(rayTriangle(ray,tris[i],d,n)&&d<best){ best=d; out.hit=true; out.node=nullptr; out.nodeId=0; out.distance=d; out.position=ray.origin+ray.direction*d; out.normal=n; out.triangleId=i; out.trianglesTested=tris.size(); out.queryTimeMs=0.0; } } out.queryTimeMs=std::chrono::duration<double,std::milli>(std::chrono::high_resolution_clock::now()-t0).count(); return out; }
SceneRayHit NSceneCollisionManager::screenPick(float x,float y) const { if(!m_camera) return {}; float nx=(2*x)-1.0F; float ny=1.0F-(2*y); math::Vector3 dir{nx*0.6F,ny*0.6F,-1.0F}; dir=dir.normalized(); math::Ray ray{m_camera->transform().position,dir}; return rayPick(ray,1000.0F); }
bool NSceneCollisionManager::intersectsAABB(const math::AABB& a,const math::AABB& b) const { return (a.min.x<=b.max.x&&a.max.x>=b.min.x)&&(a.min.y<=b.max.y&&a.max.y>=b.min.y)&&(a.min.z<=b.max.z&&a.max.z>=b.min.z); }
bool NSceneCollisionManager::intersectsSphere(const math::Sphere& a,const math::Sphere& b) const { return (a.center-b.center).length() <= (a.radius+b.radius); }
SceneRayHit NSceneCollisionManager::queryGround(const math::Vector3& p,float maxDistance) const { return rayPick(math::Ray{p,{0,-1,0}},maxDistance); }
void SceneManager::update(float dt){ m_animatorController.update(dt); if(m_collisionManager){ std::vector<std::shared_ptr<ISceneNode>> stack{m_root}; while(!stack.empty()){ auto n=stack.back(); stack.pop_back(); n->onUpdate(dt); for(auto& c:n->children()) stack.push_back(c);} } m_root->updateWorldRecursive(math::Matrix4::identity()); }
}
