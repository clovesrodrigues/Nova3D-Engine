#pragma once

#include <memory>
#include <Nova3D/Graphics/IRenderContext.hpp>
#include <Nova3D/Graphics/IRenderer.hpp>

using SDL_GLContext = void*;

namespace nova3d::graphics {

class OpenGLShader final : public IShader {
public:
    ~OpenGLShader() override;
    bool compile(const std::string& vertexSource, const std::string& fragmentSource) override;
    void bind() override;
    void unbind() override;
    bool setUniformFloat(const std::string& name, float value) override;
    bool setUniformVec4(const std::string& name, float x, float y, float z, float w) override;
private:
    unsigned int m_program = 0;
};

class OpenGLTexture final : public ITexture { public: ~OpenGLTexture() override; bool upload(const void*, int, int, int) override; void bind(std::uint32_t) override; void unbind() override; void setFilter(TextureFilter, TextureFilter) override; void setWrap(TextureWrap, TextureWrap) override; int width() const override { return m_width; } int height() const override { return m_height; } private: unsigned int m_id=0; int m_width=0; int m_height=0;};
class OpenGLVertexBuffer final : public IVertexBuffer { public: ~OpenGLVertexBuffer() override; bool upload(const void*, std::size_t) override; void bind() override; void unbind() override; private: unsigned int m_id=0; unsigned int m_vao=0;};
class OpenGLIndexBuffer final : public IIndexBuffer { public: ~OpenGLIndexBuffer() override; bool upload(const std::uint32_t*, std::size_t) override; std::size_t indexCount() const override { return m_count; } void bind() override; void unbind() override; private: unsigned int m_id=0; std::size_t m_count=0;};

class BasicMaterial final : public IMaterial { public: void setShader(const std::shared_ptr<IShader>& shader) override { m_shader=shader; } void setTexture(const std::shared_ptr<ITexture>& texture) override { m_texture=texture; } void setState(const RenderState& state) override { m_state=state; } const RenderState& state() const override { return m_state; } private: std::shared_ptr<IShader> m_shader; std::shared_ptr<ITexture> m_texture; RenderState m_state{};};

class OpenGLVideoDriver final : public IVideoDriver {
public:
    void beginFrame() override;
    void endFrame() override;
    void clear(const Color& color, bool clearDepth) override;
    void setViewport(const Viewport& viewport) override;
    void setRenderState(const RenderState& state) override;
    void drawIndexed(PrimitiveType primitive, std::size_t indexCount) override;
    std::shared_ptr<IShader> createShader() override;
    std::shared_ptr<ITexture> createTexture() override;
    std::shared_ptr<IVertexBuffer> createVertexBuffer() override;
    std::shared_ptr<IIndexBuffer> createIndexBuffer() override;
    std::shared_ptr<IMaterial> createMaterial() override;
};

class OpenGLRenderContextDesktop final : public IRenderContext {
public:
    bool initialize(platform::IWindow& window) override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void resize(int width, int height) override;
    IVideoDriver& videoDriver() override { return m_videoDriver; }
private:
    platform::IWindow* m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    OpenGLVideoDriver m_videoDriver;
};

class OpenGLESRenderContextAndroid final : public OpenGLRenderContextDesktop {};

} // namespace nova3d::graphics
