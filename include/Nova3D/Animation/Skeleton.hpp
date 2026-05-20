#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <Nova3D/Core/Math/Math.hpp>
namespace nova3d::animation {
struct BoneTransform { math::Vector3 translation{}; math::Quaternion rotation{}; math::Vector3 scale{1,1,1}; math::Matrix4 toMatrix() const; static BoneTransform blendLinear(const BoneTransform& a, const BoneTransform& b, float t); };
struct BonePose { BoneTransform local{}; math::Matrix4 global=math::Matrix4::identity(); };
struct Bone { std::string name; std::int32_t parentIndex=-1; BoneTransform bindLocal{}; math::Matrix4 inverseBindPose=math::Matrix4::identity(); };
class BoneHierarchy { public: std::int32_t addBone(Bone bone); const std::vector<Bone>& bones() const { return m_bones; } std::vector<std::int32_t> childrenOf(std::int32_t parent) const; private: std::vector<Bone> m_bones; };
class Skeleton { public: void setHierarchy(BoneHierarchy hierarchy); const BoneHierarchy& hierarchy() const { return m_hierarchy; } std::vector<BonePose> createBindPose() const; void propagateGlobalTransforms(std::vector<BonePose>& pose) const; std::vector<math::Matrix4> buildSkinMatrixPalette(const std::vector<BonePose>& pose) const; private: BoneHierarchy m_hierarchy; };
}
