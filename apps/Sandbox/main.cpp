#include <Nova3D.hpp>
#include <array>

class SandboxApp final : public nova3d::core::IApplication {
public:
    bool onInitialize(nova3d::core::NovaDevice& device) override {
        auto& driver = device.videoDriver();
        m_shader = driver.createShader();
#if defined(__ANDROID__)
        const char* vs = "#version 300 es\nlayout(location=0) in vec3 aPos;layout(location=1) in vec3 aColor;out vec3 vColor;void main(){vColor=aColor;gl_Position=vec4(aPos,1.0);}";
        const char* fs = "#version 300 es\nprecision mediump float;in vec3 vColor;out vec4 FragColor;void main(){FragColor=vec4(vColor,1.0);}";
#else
        const char* vs = "#version 330 core\nlayout(location=0) in vec3 aPos;layout(location=1) in vec3 aColor;out vec3 vColor;void main(){vColor=aColor;gl_Position=vec4(aPos,1.0);}";
        const char* fs = "#version 330 core\nin vec3 vColor;out vec4 FragColor;void main(){FragColor=vec4(vColor,1.0);}";
#endif
        if (!m_shader->compile(vs, fs)) return false;
        m_vb = driver.createVertexBuffer();
        m_ib = driver.createIndexBuffer();
        const std::array<float, 18> vertices{0.0f,0.5f,0.0f,1,0,0, -0.5f,-0.5f,0.0f,0,1,0, 0.5f,-0.5f,0.0f,0,0,1};
        const std::array<std::uint32_t, 3> indices{0,1,2};
        m_vb->upload(vertices.data(), sizeof(vertices));
        m_ib->upload(indices.data(), indices.size());
        return true;
    }
    void onUpdate(float) override {
        m_shader->bind();
        m_vb->bind();
        m_ib->bind();
        // Render commands are backend-agnostic at API boundary.
        m_driver->drawIndexed(nova3d::graphics::PrimitiveType::Triangles, m_ib->indexCount());
    }
    void onShutdown() override {}
    void setDriver(nova3d::graphics::IVideoDriver& driver) { m_driver = &driver; }
private:
    nova3d::graphics::IVideoDriver* m_driver = nullptr;
    std::shared_ptr<nova3d::graphics::IShader> m_shader;
    std::shared_ptr<nova3d::graphics::IVertexBuffer> m_vb;
    std::shared_ptr<nova3d::graphics::IIndexBuffer> m_ib;
};

int main() {
    SandboxApp app;
    nova3d::core::NovaDevice engine;
    if (!engine.initialize(app)) {
        return 1;
    }
    app.setDriver(engine.videoDriver());
    engine.run();
    return 0;
}
