#include <Nova3D/Animation/Skeleton.hpp>
namespace nova3d::animation {
math::Matrix4 BoneTransform::toMatrix() const { return math::Matrix4::translation(translation)*math::Matrix4::rotation(rotation)*math::Matrix4::scaling(scale); }
static math::Quaternion nlerp(const math::Quaternion& a,const math::Quaternion& b,float t){ math::Quaternion q{a.w+(b.w-a.w)*t,a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t}; float len=std::sqrt(q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z); if(len>0){ q.w/=len;q.x/=len;q.y/=len;q.z/=len;} return q; }
BoneTransform BoneTransform::blendLinear(const BoneTransform& a,const BoneTransform& b,float t){ return {{a.translation.x+(b.translation.x-a.translation.x)*t,a.translation.y+(b.translation.y-a.translation.y)*t,a.translation.z+(b.translation.z-a.translation.z)*t},nlerp(a.rotation,b.rotation,t),{a.scale.x+(b.scale.x-a.scale.x)*t,a.scale.y+(b.scale.y-a.scale.y)*t,a.scale.z+(b.scale.z-a.scale.z)*t}}; }
std::int32_t BoneHierarchy::addBone(Bone bone){ m_bones.push_back(std::move(bone)); return static_cast<std::int32_t>(m_bones.size()-1); }
std::vector<std::int32_t> BoneHierarchy::childrenOf(std::int32_t parent) const { std::vector<std::int32_t> c; for(std::int32_t i=0;i<static_cast<std::int32_t>(m_bones.size());++i) if(m_bones[i].parentIndex==parent) c.push_back(i); return c; }
void Skeleton::setHierarchy(BoneHierarchy hierarchy){ m_hierarchy=std::move(hierarchy); }
std::vector<BonePose> Skeleton::createBindPose() const { std::vector<BonePose> p; for(const auto& b:m_hierarchy.bones()) p.push_back({b.bindLocal,math::Matrix4::identity()}); return p; }
void Skeleton::propagateGlobalTransforms(std::vector<BonePose>& pose) const { const auto& bones=m_hierarchy.bones(); for(size_t i=0;i<bones.size();++i){ auto local=pose[i].local.toMatrix(); pose[i].global = bones[i].parentIndex>=0 ? pose[static_cast<size_t>(bones[i].parentIndex)].global*local : local; } }
std::vector<math::Matrix4> Skeleton::buildSkinMatrixPalette(const std::vector<BonePose>& pose) const { std::vector<math::Matrix4> out; const auto& bones=m_hierarchy.bones(); out.reserve(bones.size()); for(size_t i=0;i<bones.size();++i) out.push_back(pose[i].global*bones[i].inverseBindPose); return out; }
}
