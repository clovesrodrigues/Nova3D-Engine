#include <Nova3D/Scene/NGfx/NMeshManagement.hpp>

#include <limits>

namespace N {

namespace {
NAABBox nMergeBounds(const NAABBox& a, const NAABBox& b) {
    return {
        {std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y), std::min(a.min.z, b.min.z)},
        {std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y), std::max(a.max.z, b.max.z)},
    };
}
}

void NSMesh::nAddMeshBuffer(std::shared_ptr<NIMeshBuffer> meshBuffer) {
    if (!meshBuffer) {
        return;
    }
    m_buffers.push_back(std::move(meshBuffer));
    nRecalculateBoundingBox();
}

void NSMesh::nRecalculateBoundingBox() {
    if (m_buffers.empty()) {
        m_bounds = {};
        return;
    }

    m_bounds = m_buffers.front()->nGetBoundingBox();
    for (std::size_t i = 1; i < m_buffers.size(); ++i) {
        m_bounds = nMergeBounds(m_bounds, m_buffers[i]->nGetBoundingBox());
    }
}

void NSMeshCache::nAddMesh(std::string name, std::shared_ptr<NIMesh> mesh) {
    if (name.empty() || !mesh) {
        return;
    }
    m_cache[std::move(name)] = std::make_unique<std::shared_ptr<NIMesh>>(std::move(mesh));
}

std::shared_ptr<NIMesh> NSMeshCache::nGetMeshByName(std::string_view name) const {
    const auto it = m_cache.find(std::string(name));
    if (it == m_cache.end() || !it->second) {
        return nullptr;
    }
    return *it->second;
}

} // namespace N
