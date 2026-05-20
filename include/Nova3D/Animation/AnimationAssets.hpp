#pragma once
#include <string>
#include <vector>
#include <memory>
#include <Nova3D/Animation/Skeleton.hpp>
#include <Nova3D/Animation/AnimationClip.hpp>
#include <Nova3D/Scene/MeshData.hpp>

namespace nova3d::animation {
struct SkeletonAsset { std::string name; std::shared_ptr<Skeleton> skeleton; std::vector<math::Matrix4> bindPoses; };
struct AnimationAsset { std::string name; std::vector<std::shared_ptr<AnimationClip>> clips; std::string skeletonReferenceGuid; };
struct SkinnedMeshAsset { std::string name; scene::MeshCPUData mesh; std::vector<std::array<int,4>> boneIndices; std::vector<std::array<float,4>> boneWeights; std::string skeletonReferenceGuid; };
}
