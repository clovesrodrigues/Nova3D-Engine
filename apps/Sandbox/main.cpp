#include <Nova3D.hpp>
#include <Nova3D/Scene/Scene.hpp>
#include <Nova3D/Graphics/RenderPipeline/ForwardRenderer.hpp>
#include <Nova3D/Assets/Importers/OBJImporter.hpp>
#include <Nova3D/Animation/AnimationState.hpp>

class SandboxApp final : public nova3d::core::IApplication {
public:
    bool onInitialize(nova3d::core::NovaDevice& device) override {
        auto& driver = device.videoDriver();
        m_scene = std::make_unique<nova3d::scene::SceneManager>(driver);
        m_renderer = std::make_unique<nova3d::graphics::ForwardRenderer>(driver);
        m_camera = m_scene->createCamera();
        m_camera->transform().position = {0,2,4};
        m_camera->setPerspective(60.0F, 16.0F/9.0F, 0.1F, 500.0F);
        m_camera->setViewport({0,0,1280,720});
        m_camera->lookAt({0,0,0});
        m_scene->setActiveCamera(m_camera);
        nova3d::assets::OBJImporter importer; auto model=importer.importFromFile("assets/models/cube.obj"); auto mesh=model?nova3d::scene::createMeshFromModelAsset(*model):nova3d::scene::createSimpleTriangleMesh();
        m_meshNode = m_scene->createMeshNode(mesh);
        auto staticNode=m_scene->createMeshNode(mesh); staticNode->transform().position={2.0F,0,0}; staticNode->transform().markDirty();
        m_scene->createLightNode(std::make_shared<nova3d::scene::DirectionalLight>());
        auto skeleton=std::make_shared<nova3d::animation::Skeleton>(); nova3d::animation::BoneHierarchy h; h.addBone({"root",-1,{},{}}); skeleton->setHierarchy(h);
        auto clip=std::make_shared<nova3d::animation::AnimationClip>("spin"); clip->setDuration(2.0F); nova3d::animation::AnimationTrack t; nova3d::animation::AnimationChannel ch; ch.boneIndex=0; ch.keyframes.push_back({0.0F,{{0,0,0},nova3d::math::Quaternion::fromAxisAngle({0,1,0},0),{1,1,1}}}); ch.keyframes.push_back({2.0F,{{0,0,0},nova3d::math::Quaternion::fromAxisAngle({0,1,0},nova3d::math::kPi*2.0F),{1,1,1}}}); t.channels.push_back(ch); clip->tracks().push_back(t);
        m_animController.setSkeleton(skeleton); nova3d::animation::AnimationPlayer p; p.setClip(clip); p.state().play(); m_animController.addPlayer(std::move(p));
        return true;
    }
    void onUpdate(float dt) override {
        m_animController.update(dt);
        m_angle += dt;
        m_meshNode->transform().rotation = nova3d::math::Quaternion::fromAxisAngle({0,1,0}, m_angle);
        m_meshNode->transform().markDirty();
        m_camera->transform().position = {std::sin(m_angle)*4.0F,2.0F,std::cos(m_angle)*4.0F}; m_camera->transform().markDirty(); m_camera->lookAt({0,0,0});
        m_scene->update(dt);
        m_renderer->render(*m_scene);
    }
private:
    float m_angle = 0.0F;
    std::unique_ptr<nova3d::scene::SceneManager> m_scene;
    std::unique_ptr<nova3d::graphics::ForwardRenderer> m_renderer;
    std::shared_ptr<nova3d::scene::CameraSceneNode> m_camera;
    std::shared_ptr<nova3d::scene::MeshSceneNode> m_meshNode;
    nova3d::animation::AnimationController m_animController;
};
int main(){ SandboxApp app; nova3d::core::NovaDevice engine; if(!engine.initialize(app)) return 1; engine.run(); return 0; }
