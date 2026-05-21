#include <Nova3D.hpp>
#include <Nova3D/Scene/Scene.hpp>
#include <Nova3D/Graphics/RenderPipeline/ForwardRenderer.hpp>
#include <Nova3D/Animation/AnimationState.hpp>
#include <Nova3D/Assets/Importers/FBXImporter.hpp>
#include <Nova3D/GUI/GUI.hpp>
#include <Nova3D/Editor/RuntimeUIFoundation.hpp>
#include <Nova3D/Runtime/RuntimeFoundation.hpp>
#include <Nova3D/Runtime/EngineInfrastructure.hpp>
#include <Nova3D/Memory/MemoryFoundation.hpp>

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
                nova3d::scene::NSpawnReport reportA{};
        nova3d::scene::NPlacementOptions base{};
        base.initialPosition = {0.0F, 0.0F, 0.0F};
        base.debugName = "pivot-center";
        m_scene->spawnModel("assets/models/cube.obj", base, &reportA);
        nova3d::core::Logger::info("[Test A] center nodes=" + std::to_string(reportA.createdNodeCount));

        auto bottom = base; bottom.initialPosition = {2.0F, 0.0F, 0.0F}; bottom.pivotMode = nova3d::scene::NPivotMode::BottomCenter;
        nova3d::scene::NSpawnReport reportB{}; m_scene->spawnModel("assets/models/cube.obj", bottom, &reportB);
        nova3d::core::Logger::info("[Test B] bottom pivot world y=" + std::to_string(reportB.computedPivotWorld.y));

        auto top = base; top.initialPosition = {-2.0F, 0.0F, 0.0F}; top.pivotMode = nova3d::scene::NPivotMode::TopCenter;
        nova3d::scene::NSpawnReport reportC{}; m_scene->spawnModel("assets/models/cube.obj", top, &reportC);

        auto custom = base; custom.initialPosition = {0.0F, 0.0F, 2.0F}; custom.pivotMode = nova3d::scene::NPivotMode::Custom; custom.customPivotOffset = {0.0F, -1.0F, 0.0F};
        nova3d::scene::NSpawnReport reportD{}; m_scene->spawnModel("assets/models/cube.obj", custom, &reportD);

        auto look = base; look.initialPosition = {0.0F, 0.0F, -2.0F}; look.useLookAt = true; look.lookAtTarget = {0.0F, 0.0F, 0.0F};
        nova3d::scene::NSpawnReport reportE{}; m_scene->spawnModel("assets/models/cube.obj", look, &reportE);
        nova3d::core::Logger::info("[Test E] look-at warnings=" + std::to_string(reportE.warnings.size()));

        m_meshNode = m_scene->createMeshNode(nova3d::scene::createSimpleTriangleMesh());
        auto staticNode=m_scene->createMeshNode(nova3d::scene::createSimpleTriangleMesh()); staticNode->transform().position={2.0F,0,0}; staticNode->transform().markDirty();
        m_scene->createLightNode(std::make_shared<nova3d::scene::DirectionalLight>());

        nova3d::reflection::TypeRegistry::instance().registerType({"MeshSceneNode","SceneNode",{{"visible","bool",{true,0,0,"Rendering",true,true}}},{}});
        m_objId = m_registry.registerObject(m_meshNode);
        auto entry=nova3d::editor::RuntimeIntrospection::inspect(m_objId,"MeshSceneNode");
        nova3d::core::Logger::info("Introspection properties: "+std::to_string(entry.properties.size()));
        m_savedSceneJson=nova3d::scene::SceneSerializer::saveToJson(*m_scene);
        m_prefabInstance.prefab={"MeshNode",m_savedSceneJson};
        const auto prefabJson=nova3d::scene::PrefabSerializer::toJson(m_prefabInstance);
        nova3d::core::Logger::info("Scene JSON bytes: "+std::to_string(m_savedSceneJson.size())+", Prefab JSON bytes: "+std::to_string(prefabJson.size()));

        if (m_audio.initialize() && m_clip.loadFromFile("assets/audio/test.wav")) { m_voice = m_audio.play(m_clip, 0.6F, true); }
        m_guiManager = std::make_unique<nova3d::gui::GUIManager>(std::make_shared<NullGUIRenderer>());
        m_runtimeUi = nova3d::editor::RuntimeUIFoundation::build(m_guiManager->context());
        auto btn = std::make_shared<nova3d::gui::Button>(); btn->text = "Play/Pause"; btn->rect={30,330,140,30}; btn->onClick=[this](){m_playback=!m_playback;}; m_runtimeUi.assetBrowser->addChild(btn);

        m_scheduler.start(2);
        m_events.subscribe("resource.loaded", [this](const nova3d::runtime::EngineEvent&){ m_runtimeStats.frame.tasksExecuted++; });
        m_asyncLoader = std::make_unique<nova3d::runtime::AsyncResourceLoader>(m_scheduler);
        nova3d::runtime::AsyncResourceRequest req{"sandbox.mesh"};
        m_asyncLoader->enqueue(req,[this](){ m_streaming.enqueue({"sandbox.chunk", nova3d::runtime::StreamingPriority::Normal}); m_uploads.enqueue({{"mesh-upload",{}}, nova3d::runtime::TaskPriority::Normal}); m_eventQueue.push({"resource.loaded","sandbox.mesh"}); });
        return true;
    }
    void onUpdate(float dt) override {
        nova3d::runtime::ProfileScope profileScope(m_profiler, "sandbox.update");
        if (m_playback) m_animController.update(dt); m_debug.newFrame(dt); m_guiManager->tick();
        m_events.pump(m_eventQueue);
        m_angle += dt;
        m_meshNode->transform().rotation = nova3d::math::Quaternion::fromAxisAngle({0,1,0}, m_angle);
        m_meshNode->transform().markDirty();
        m_camera->transform().position = {std::sin(m_angle)*4.0F,2.0F,std::cos(m_angle)*4.0F}; m_camera->transform().markDirty(); m_camera->lookAt({0,0,0});
        m_scene->update(dt);
        m_debug.drawFrustum(m_camera->frustum(),{1,1,0,1},0.0F);
        m_debug.drawBox(m_meshNode->worldBounds(),{0,1,1,1},0.0F);
        m_renderer->render(*m_scene);
        m_runtimeStats.frame.frameTimeMs = dt*1000.0F;
        m_runtimeStats.frame.fps = dt > 0 ? 1.0F/dt : 0.0F;
    }
    void onShutdown() override { m_audio.stop(m_voice); m_audio.shutdown(); }
private:
    class NullGUIRenderer final : public nova3d::gui::GUIRenderer { public: void beginFrame() override {} void drawQuad(const nova3d::gui::Rect&, const nova3d::math::Vector4&, int) override {} void drawText(const nova3d::gui::Rect&, const std::string&, const nova3d::math::Vector4&, int) override {} void endFrame() override {} };
    float m_angle = 0.0F;
    std::string m_savedSceneJson;
    nova3d::scene::PrefabInstance m_prefabInstance;
    nova3d::core::ObjectRegistry m_registry;
    nova3d::core::ObjectId m_objId{};
    nova3d::debug::DebugRenderer m_debug;
    std::unique_ptr<nova3d::scene::SceneManager> m_scene;
    std::unique_ptr<nova3d::graphics::ForwardRenderer> m_renderer;
    std::shared_ptr<nova3d::scene::CameraSceneNode> m_camera;
    std::shared_ptr<nova3d::scene::MeshSceneNode> m_meshNode;
    nova3d::animation::AnimationController m_animController;
    bool m_playback = true;
    nova::audio::NAudioDevice m_audio;
    nova::audio::NAudioClip m_clip;
    nova::audio::NAudioVoiceHandle m_voice{};
    nova3d::runtime::TaskScheduler m_scheduler;
    std::unique_ptr<nova3d::runtime::AsyncResourceLoader> m_asyncLoader;
    nova3d::runtime::Profiler m_profiler;
    nova3d::runtime::EventBus m_events;
    nova3d::runtime::EventQueue m_eventQueue;
    nova3d::runtime::StreamingManager m_streaming;
    nova3d::runtime::GPUUploadQueue m_uploads;
    nova3d::runtime::FrameStatistics m_runtimeStats;
    std::unique_ptr<nova3d::gui::GUIManager> m_guiManager;
    nova3d::editor::RuntimeUIFoundation m_runtimeUi;
};
int main(){ SandboxApp app; nova3d::core::NovaDevice engine; if(!engine.initialize(app)) return 1; engine.run(); return 0; }
