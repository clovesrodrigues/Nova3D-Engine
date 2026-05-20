#include <Nova3D/Assets/Importers/FBXImporter.hpp>
namespace nova3d::assets {
std::shared_ptr<ImportedFBXScene> FBXImporter::importFromFile(const std::string& path) const { if(!m_backend) return {}; return m_backend->importScene(path); }
std::shared_ptr<ImportedFBXScene> StubFBXImporterBackend::importScene(const std::string& path) const {
    auto out=std::make_shared<ImportedFBXScene>();
    out->meshes.push_back(std::make_shared<ModelAsset>());
    animation::BoneHierarchy h; h.addBone({"root",-1,{},{}}); h.addBone({"spine",0,{},{}});
    auto skeleton=std::make_shared<animation::Skeleton>(); skeleton->setHierarchy(h);
    animation::SkeletonAsset sk; sk.name=path+":skeleton"; sk.skeleton=skeleton; sk.bindPoses.resize(2); out->skeletons.push_back(sk);
    auto clip=std::make_shared<animation::AnimationClip>("imported_clip"); clip->setDuration(1.0F);
    animation::AnimationTrack track; animation::AnimationChannel ch; ch.boneIndex=1; ch.keyframes.push_back({0.0F,{{0,0,0},math::Quaternion::identity(),{1,1,1}}}); ch.keyframes.push_back({1.0F,{{0,0.1F,0},math::Quaternion::identity(),{1,1,1}}}); track.channels.push_back(ch); clip->tracks().push_back(track);
    animation::AnimationAsset a; a.name=path+":anim"; a.clips.push_back(clip); out->animations.push_back(a);
    out->cameras.push_back("MainCamera"); out->lights.push_back("Sun");
    return out;
}
}
