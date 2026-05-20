#include <Nova3D.hpp>

class SandboxApp final : public nova3d::core::IApplication {
public:
    bool onInitialize(nova3d::core::NovaDevice& device) override {
        auto& driver = device.videoDriver();
        m_scene = std::make_unique<nova3d::scene::SceneManager>(driver);
        m_camera = m_scene->createCamera();
        m_camera->transform().position = {0,0,2};
        m_camera->setPerspective(60.0F, 1.6F, 0.1F, 100.0F);
        m_camera->lookAt({0,0,0});
        m_scene->setActiveCamera(m_camera);
        m_meshNode = m_scene->createMeshNode(nova3d::scene::createSimpleTriangleMesh());
        return true;
    }
    void onUpdate(float dt) override {
        m_angle += dt;
        m_meshNode->transform().rotation = nova3d::math::Quaternion::fromAxisAngle({0,0,1}, m_angle);
        m_meshNode->transform().markDirty();
        m_scene->update(dt);
        m_scene->render();
    }
    void onShutdown() override {}
private:
    float m_angle = 0.0F;
    std::unique_ptr<nova3d::scene::SceneManager> m_scene;
    std::shared_ptr<nova3d::scene::CameraSceneNode> m_camera;
    std::shared_ptr<nova3d::scene::MeshSceneNode> m_meshNode;
};

int main() {
    SandboxApp app;
    nova3d::core::NovaDevice engine;
    if (!engine.initialize(app)) return 1;
    engine.run();
    return 0;
}
