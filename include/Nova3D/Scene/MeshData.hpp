#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <Nova3D/Core/Math/Math.hpp>
#include <memory>
namespace nova3d::scene {
struct MeshVertex { math::Vector3 position{}; math::Vector3 normal{}; math::Vector2 uv{}; math::Vector4 color{1,1,1,1}; std::array<std::uint16_t,4> boneIndices{0,0,0,0}; std::array<float,4> boneWeights{1,0,0,0}; };
struct MeshSection { std::uint32_t indexOffset=0; std::uint32_t indexCount=0; std::uint32_t vertexOffset=0; std::string materialSlot; math::AABB bounds{}; std::int32_t smoothingGroup=-1; };
using SubMesh = MeshSection;
struct MeshCPUData { std::vector<MeshVertex> vertices; std::vector<std::uint32_t> indices; std::vector<MeshSection> sections; math::AABB bounds{}; };
}

namespace nova3d::animation { class Skeleton; }
namespace nova3d::scene { struct SkinnedMeshBinding { std::shared_ptr<animation::Skeleton> skeleton; std::vector<math::Matrix4> matrixPalette; bool gpuSkinningPrepared=false; }; }
