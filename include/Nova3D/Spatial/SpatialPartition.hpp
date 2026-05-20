#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Core/Math/Math.hpp>
namespace nova3d::spatial {
struct SpatialNode { std::size_t id=0; math::AABB bounds{}; void* userData=nullptr; bool dynamic=true; };
class SpatialPartition { public: virtual ~SpatialPartition()=default; virtual void insert(const SpatialNode& node)=0; virtual void remove(std::size_t id)=0; virtual void update(const SpatialNode& node)=0; virtual std::vector<SpatialNode> queryFrustum(const math::Frustum& frustum) const=0; };
class OctreePartition final : public SpatialPartition { public: explicit OctreePartition(math::AABB worldBounds):m_worldBounds(worldBounds){} void insert(const SpatialNode& node) override; void remove(std::size_t id) override; void update(const SpatialNode& node) override; std::vector<SpatialNode> queryFrustum(const math::Frustum& frustum) const override; private: math::AABB m_worldBounds{}; std::vector<SpatialNode> m_nodes; };
class BVHPartition final : public SpatialPartition { public: void insert(const SpatialNode& node) override; void remove(std::size_t id) override; void update(const SpatialNode& node) override; std::vector<SpatialNode> queryFrustum(const math::Frustum& frustum) const override; private: std::vector<SpatialNode> m_nodes; };
}
