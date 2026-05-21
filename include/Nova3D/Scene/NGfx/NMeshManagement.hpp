#pragma once

#include <Nova3D/Core/Math/Math.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include <ostream>

namespace N {

using NVector3 = nova3d::math::Vector3;
using NMatrix4 = nova3d::math::Matrix4;
using NAABBox = nova3d::math::AABB;

enum class E_HARDWARE_MAPPING : std::uint8_t { Static, Dynamic, Stream };

class NIMeshBuffer {
public:
    virtual ~NIMeshBuffer() = default;
    virtual std::size_t nGetVertexCount() const = 0;
    virtual std::size_t nGetIndexCount() const = 0;
    virtual const NAABBox& nGetBoundingBox() const = 0;
    virtual const void* nGetVertexDataRaw() const = 0;
    virtual const std::uint32_t* nGetIndexData() const = 0;
};

template <typename TVertex>
class NCMeshBuffer : public NIMeshBuffer {
public:
    using VertexType = TVertex;

    std::size_t nGetVertexCount() const override { return m_vertices.size(); }
    std::size_t nGetIndexCount() const override { return m_indices.size(); }
    const NAABBox& nGetBoundingBox() const override { return m_bounds; }
    const void* nGetVertexDataRaw() const override { return m_vertices.data(); }
    const std::uint32_t* nGetIndexData() const override { return m_indices.data(); }

    const std::vector<TVertex>& nGetVertices() const { return m_vertices; }
    const std::vector<std::uint32_t>& nGetIndices() const { return m_indices; }
    std::vector<TVertex>& nGetVertices() { return m_vertices; }
    std::vector<std::uint32_t>& nGetIndices() { return m_indices; }

    void nSetBoundingBox(const NAABBox& bounds) { m_bounds = bounds; }

protected:
    std::vector<TVertex> m_vertices;
    std::vector<std::uint32_t> m_indices;
    NAABBox m_bounds{};
};

class NIDynamicMeshBuffer : public NIMeshBuffer {
public:
    virtual void nSetHardwareMapping(E_HARDWARE_MAPPING mapping) = 0;
    virtual E_HARDWARE_MAPPING nGetHardwareMapping() const = 0;
    virtual void nSetDirty(bool dirty = true) = 0;
    virtual bool nIsDirty() const = 0;
};

template <typename TVertex>
class NCDynamicMeshBuffer final : public NIDynamicMeshBuffer, public NCMeshBuffer<TVertex> {
public:
    void nSetHardwareMapping(E_HARDWARE_MAPPING mapping) override { m_mapping = mapping; }
    E_HARDWARE_MAPPING nGetHardwareMapping() const override { return m_mapping; }
    void nSetDirty(bool dirty = true) override { m_dirty = dirty; }
    bool nIsDirty() const override { return m_dirty; }

    std::size_t nGetVertexCount() const override { return NCMeshBuffer<TVertex>::nGetVertexCount(); }
    std::size_t nGetIndexCount() const override { return NCMeshBuffer<TVertex>::nGetIndexCount(); }
    const NAABBox& nGetBoundingBox() const override { return NCMeshBuffer<TVertex>::nGetBoundingBox(); }
    const void* nGetVertexDataRaw() const override { return NCMeshBuffer<TVertex>::nGetVertexDataRaw(); }
    const std::uint32_t* nGetIndexData() const override { return NCMeshBuffer<TVertex>::nGetIndexData(); }

private:
    E_HARDWARE_MAPPING m_mapping{E_HARDWARE_MAPPING::Dynamic};
    bool m_dirty{true};
};

class NSSharedMeshBuffer final : public NIMeshBuffer {
public:
    explicit NSSharedMeshBuffer(std::shared_ptr<NIMeshBuffer> sharedBuffer) : m_sharedBuffer(std::move(sharedBuffer)) {}

    std::size_t nGetVertexCount() const override { return m_sharedBuffer ? m_sharedBuffer->nGetVertexCount() : 0; }
    std::size_t nGetIndexCount() const override { return m_sharedBuffer ? m_sharedBuffer->nGetIndexCount() : 0; }
    const NAABBox& nGetBoundingBox() const override { return m_sharedBuffer->nGetBoundingBox(); }
    const void* nGetVertexDataRaw() const override { return m_sharedBuffer ? m_sharedBuffer->nGetVertexDataRaw() : nullptr; }
    const std::uint32_t* nGetIndexData() const override { return m_sharedBuffer ? m_sharedBuffer->nGetIndexData() : nullptr; }

    const std::shared_ptr<NIMeshBuffer>& nGetSource() const { return m_sharedBuffer; }

private:
    std::shared_ptr<NIMeshBuffer> m_sharedBuffer;
};

class NSSkinMeshBuffer final : public NIMeshBuffer {
public:
    explicit NSSkinMeshBuffer(std::shared_ptr<NIMeshBuffer> baseBuffer) : m_baseBuffer(std::move(baseBuffer)) {}

    std::size_t nGetVertexCount() const override { return m_baseBuffer ? m_baseBuffer->nGetVertexCount() : 0; }
    std::size_t nGetIndexCount() const override { return m_baseBuffer ? m_baseBuffer->nGetIndexCount() : 0; }
    const NAABBox& nGetBoundingBox() const override { return m_baseBuffer->nGetBoundingBox(); }
    const void* nGetVertexDataRaw() const override { return m_baseBuffer ? m_baseBuffer->nGetVertexDataRaw() : nullptr; }
    const std::uint32_t* nGetIndexData() const override { return m_baseBuffer ? m_baseBuffer->nGetIndexData() : nullptr; }

    void nSetBoneIndices(std::vector<std::array<std::uint16_t, 4>> indices) { m_boneIndices = std::move(indices); }
    void nSetBoneWeights(std::vector<std::array<float, 4>> weights) { m_boneWeights = std::move(weights); }
    const std::vector<std::array<std::uint16_t, 4>>& nGetBoneIndices() const { return m_boneIndices; }
    const std::vector<std::array<float, 4>>& nGetBoneWeights() const { return m_boneWeights; }

private:
    std::shared_ptr<NIMeshBuffer> m_baseBuffer;
    std::vector<std::array<std::uint16_t, 4>> m_boneIndices;
    std::vector<std::array<float, 4>> m_boneWeights;
};

class NIMesh {
public:
    virtual ~NIMesh() = default;
    virtual std::size_t nGetMeshBufferCount() const = 0;
    virtual std::shared_ptr<NIMeshBuffer> nGetMeshBuffer(std::size_t index) const = 0;
    virtual const NAABBox& nGetBoundingBox() const = 0;
};

class NSMesh final : public NIMesh {
public:
    std::size_t nGetMeshBufferCount() const override { return m_buffers.size(); }
    std::shared_ptr<NIMeshBuffer> nGetMeshBuffer(std::size_t index) const override { return m_buffers.at(index); }
    const NAABBox& nGetBoundingBox() const override { return m_bounds; }

    void nAddMeshBuffer(std::shared_ptr<NIMeshBuffer> meshBuffer);
    void nRecalculateBoundingBox();

private:
    std::vector<std::shared_ptr<NIMeshBuffer>> m_buffers;
    NAABBox m_bounds{};
};

class NIMeshCache {
public:
    virtual ~NIMeshCache() = default;
    virtual void nAddMesh(std::string name, std::shared_ptr<NIMesh> mesh) = 0;
    virtual std::shared_ptr<NIMesh> nGetMeshByName(std::string_view name) const = 0;
};

class NSMeshCache final : public NIMeshCache {
public:
    void nAddMesh(std::string name, std::shared_ptr<NIMesh> mesh) override;
    std::shared_ptr<NIMesh> nGetMeshByName(std::string_view name) const override;

private:
    std::unordered_map<std::string, std::unique_ptr<std::shared_ptr<NIMesh>>> m_cache;
};

class NIMeshLoader {
public:
    virtual ~NIMeshLoader() = default;
    virtual bool nIsALoadableFileExtension(std::string_view filename) const = 0;
    virtual std::shared_ptr<NIMesh> nLoadMesh(std::istream& stream, std::string_view debugName) const = 0;
};

class NIMeshManipulator {
public:
    virtual ~NIMeshManipulator() = default;
    virtual void nFlipSurfaces(NIMesh& mesh) const = 0;
    virtual void nRecalculateNormals(NIMesh& mesh) const = 0;
    virtual void nScaleMesh(NIMesh& mesh, float uniformScale) const = 0;
    virtual void nTransformMesh(NIMesh& mesh, const NMatrix4& transform) const = 0;
};

class NIMeshWriter {
public:
    virtual ~NIMeshWriter() = default;
    virtual bool nWriteMesh(std::ostream& stream, const NIMesh& mesh) const = 0;
};

} // namespace N
