#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <Nova3D/Graphics/RendererTypes.hpp>

namespace nova3d::graphics {

class ITexture;
class IShader;
class IMaterial;
class IVertexBuffer;
class IIndexBuffer;
class IRenderTarget;

class ITexture {
public:
    virtual ~ITexture() = default;
    virtual bool upload(const void* pixels, int width, int height, int channels) = 0;
    virtual void bind(std::uint32_t slot) = 0;
    virtual void unbind() = 0;
    virtual void setFilter(TextureFilter minFilter, TextureFilter magFilter) = 0;
    virtual void setWrap(TextureWrap u, TextureWrap v) = 0;
    virtual int width() const = 0;
    virtual int height() const = 0;
};

class IShader {
public:
    virtual ~IShader() = default;
    virtual bool compile(const std::string& vertexSource, const std::string& fragmentSource) = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool setUniformFloat(const std::string& name, float value) = 0;
    virtual bool setUniformVec4(const std::string& name, float x, float y, float z, float w) = 0;
};

class IVertexBuffer {
public:
    virtual ~IVertexBuffer() = default;
    virtual bool upload(const void* data, std::size_t bytes) = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
};

class IIndexBuffer {
public:
    virtual ~IIndexBuffer() = default;
    virtual bool upload(const std::uint32_t* data, std::size_t count) = 0;
    virtual std::size_t indexCount() const = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
};

class IMaterial {
public:
    virtual ~IMaterial() = default;
    virtual void setShader(const std::shared_ptr<IShader>& shader) = 0;
    virtual void setTexture(const std::shared_ptr<ITexture>& texture) = 0;
    virtual void setState(const RenderState& state) = 0;
    virtual const RenderState& state() const = 0;
};

class IRenderTarget {
public:
    virtual ~IRenderTarget() = default;
};

class IVideoDriver {
public:
    virtual ~IVideoDriver() = default;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear(const Color& color, bool clearDepth) = 0;
    virtual void setViewport(const Viewport& viewport) = 0;
    virtual void setRenderState(const RenderState& state) = 0;
    virtual void drawIndexed(PrimitiveType primitive, std::size_t indexCount) = 0;
    virtual std::shared_ptr<IShader> createShader() = 0;
    virtual std::shared_ptr<ITexture> createTexture() = 0;
    virtual std::shared_ptr<IVertexBuffer> createVertexBuffer() = 0;
    virtual std::shared_ptr<IIndexBuffer> createIndexBuffer() = 0;
    virtual std::shared_ptr<IMaterial> createMaterial() = 0;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual IVideoDriver& videoDriver() = 0;
};

} // namespace nova3d::graphics
