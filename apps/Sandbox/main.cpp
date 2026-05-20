#include <Nova3D.hpp>
#include <Nova3D/IO/FileSystem.hpp>
#include <Nova3D/Assets/AssetTypes.hpp>
#include <Nova3D/Resources/ResourceSystem.hpp>

class SandboxApp final : public nova3d::core::IApplication {
public:
    bool onInitialize(nova3d::core::NovaDevice& device) override {
        using namespace nova3d;
        resources::ResourceLoader loader;
        loader.registerFactory("glsl", [&](const std::string& path){ return std::make_shared<assets::ShaderAsset>(path, m_fs, device.videoDriver()); });
        loader.registerFactory("png", [&](const std::string& path){ return std::make_shared<assets::TextureAsset>(path, m_fs, device.videoDriver()); });
        loader.registerFactory("obj", [&](const std::string& path){ return std::make_shared<assets::MeshAsset>(path, m_fs); });
        m_resources = std::make_unique<resources::ResourceManager>(std::move(loader));
        m_shader = m_resources->load<assets::ShaderAsset>("assets/shaders/basic.glsl");
        auto& driver = device.videoDriver();
        m_scene = std::make_unique<nova3d::scene::SceneManager>(driver);
        m_camera = m_scene->createCamera();
        m_camera->transform().position = {0,0,2};
        m_camera->setPerspective(60.0F, 1.6F, 0.1F, 100.0F);
        m_camera->lookAt({0,0,0});
        m_scene->setActiveCamera(m_camera);
        m_meshNode = m_scene->createMeshNode(nova3d::scene::createSimpleTriangleMesh());
        if (m_shader) { m_shader->setMetadata("demo", "sandbox"); }
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
    nova3d::io::FileSystem m_fs;
    std::unique_ptr<nova3d::resources::ResourceManager> m_resources;
    std::shared_ptr<nova3d::assets::ShaderAsset> m_shader;
};

int main() {
    SandboxApp app;
    nova3d::core::NovaDevice engine;
    if (!engine.initialize(app)) return 1;
    engine.run();
    return 0;
}
