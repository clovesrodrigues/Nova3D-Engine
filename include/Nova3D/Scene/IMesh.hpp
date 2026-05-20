#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <Nova3D/Core/Math/Math.hpp>
#include <Nova3D/Graphics/RendererTypes.hpp>

namespace nova3d::graphics { class IMaterial; }
namespace nova3d::scene {

struct Vertex { math::Vector3 position{}; math::Vector3 normal{}; math::Vector2 uv{}; math::Vector4 color{1,1,1,1}; };

class IMeshBuffer {
public:
    virtual ~IMeshBuffer() = default;
    virtual const std::vector<Vertex>& vertices() const = 0;
    virtual const std::vector<std::uint32_t>& indices() const = 0;
    virtual graphics::PrimitiveType primitiveType() const = 0;
    virtual const math::AABB& boundingBox() const = 0;
    virtual std::shared_ptr<graphics::IMaterial> material() const = 0;
};

class IMesh {
public:
    virtual ~IMesh() = default;
    virtual std::size_t meshBufferCount() const = 0;
    virtual std::shared_ptr<IMeshBuffer> meshBuffer(std::size_t index) const = 0;
    virtual const math::AABB& boundingBox() const = 0;
};

} // namespace
