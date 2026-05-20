#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Graphics/IRenderer.hpp>
#include <Nova3D/Scene/MeshData.hpp>
namespace nova3d::graphics {
enum class MeshUsage { Static, Dynamic };
struct MeshGPUSection { std::uint32_t indexOffset=0; std::uint32_t indexCount=0; std::string materialSlot; math::AABB bounds{}; };
class MeshGPUResource { public: bool upload(IVideoDriver& driver,const scene::MeshCPUData& cpu,MeshUsage usage); void invalidate(); bool valid() const { return m_valid; } const std::vector<MeshGPUSection>& sections() const { return m_sections; } const std::shared_ptr<IVertexBuffer>& vertexBuffer() const { return m_vb; } const std::shared_ptr<IIndexBuffer>& indexBuffer() const { return m_ib; } private: std::shared_ptr<IVertexBuffer> m_vb; std::shared_ptr<IIndexBuffer> m_ib; std::vector<MeshGPUSection> m_sections; bool m_valid=false; };
class MeshUploadManager { public: std::shared_ptr<MeshGPUResource> createOrUpdate(IVideoDriver& driver,const scene::MeshCPUData& cpu,MeshUsage usage); void cleanupInvalid(); private: std::vector<std::weak_ptr<MeshGPUResource>> m_resources; };
}
