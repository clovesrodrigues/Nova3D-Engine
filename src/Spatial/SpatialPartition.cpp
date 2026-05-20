#include <algorithm>
#include <Nova3D/Spatial/SpatialPartition.hpp>
namespace nova3d::spatial {
void OctreePartition::insert(const SpatialNode& node){ m_nodes.push_back(node);} 
void OctreePartition::remove(std::size_t id){ m_nodes.erase(std::remove_if(m_nodes.begin(),m_nodes.end(),[&](const auto& n){return n.id==id;}),m_nodes.end()); }
void OctreePartition::update(const SpatialNode& node){ remove(node.id); insert(node);} 
std::vector<SpatialNode> OctreePartition::queryFrustum(const math::Frustum& frustum) const { std::vector<SpatialNode> out; for(const auto& n:m_nodes) if(frustum.intersects(n.bounds)) out.push_back(n); return out; }
void BVHPartition::insert(const SpatialNode& node){ m_nodes.push_back(node);} 
void BVHPartition::remove(std::size_t id){ m_nodes.erase(std::remove_if(m_nodes.begin(),m_nodes.end(),[&](const auto& n){return n.id==id;}),m_nodes.end()); }
void BVHPartition::update(const SpatialNode& node){ remove(node.id); insert(node);} 
std::vector<SpatialNode> BVHPartition::queryFrustum(const math::Frustum& frustum) const { std::vector<SpatialNode> out; for(const auto& n:m_nodes) if(frustum.intersects(n.bounds)) out.push_back(n); return out; }
}
