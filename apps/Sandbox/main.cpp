#include <Nova3D.hpp>
#include <Nova3D/Scene/Scene.hpp>
#include <Nova3D/Graphics/RenderPipeline/ForwardRenderer.hpp>
class SandboxApp final : public nova3d::core::IApplication {
public:
    bool onInitialize(nova3d::core::NovaDevice& device) override {
        auto& driver = device.videoDriver();
        m_scene = std::make_unique<nova3d::scene::SceneManager>(driver);
        m_renderer = std::make_unique<nova3d::graphics::ForwardRenderer>(driver);
        m_camera = m_scene->createCamera();
        m_camera->transform().position = {0,0,2};
        m_camera->setPerspective(60.0F, 16.0F/9.0F, 0.1F, 100.0F);
        m_camera->setViewport({0,0,1280,720});
        m_camera->lookAt({0,0,0});
        m_scene->setActiveCamera(m_camera);
        m_meshNode = m_scene->createMeshNode(nova3d::scene::createSimpleTriangleMesh());
        m_scene->createLightNode(std::make_shared<nova3d::scene::DirectionalLight>());
        auto p = std::make_shared<nova3d::scene::PointLight>(); p->range = 15.0F; m_scene->createLightNode(p);
        return true;
    }
    void onUpdate(float dt) override {
        m_angle += dt;
        m_meshNode->transform().rotation = nova3d::math::Quaternion::fromAxisAngle({0,1,0}, m_angle);
        m_meshNode->transform().markDirty();
        m_scene->update(dt);
        m_renderer->render(*m_scene);
    }
private:
    float m_angle = 0.0F;
    std::unique_ptr<nova3d::scene::SceneManager> m_scene;
    std::unique_ptr<nova3d::graphics::ForwardRenderer> m_renderer;
    std::shared_ptr<nova3d::scene::CameraSceneNode> m_camera;
    std::shared_ptr<nova3d::scene::MeshSceneNode> m_meshNode;
};
int main(){ SandboxApp app; nova3d::core::NovaDevice engine; if(!engine.initialize(app)) return 1; engine.run(); return 0; }
