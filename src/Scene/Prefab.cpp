#include <Nova3D/Scene/Prefab.hpp>
namespace nova3d::scene {
std::string PrefabSerializer::toJson(const PrefabInstance& instance){ serialization::JsonSerializer s; auto& r=s.root(); r["type"].value=instance.prefab.type; r["sourceSceneJson"].value=instance.prefab.sourceSceneJson; return s.toJsonString(); }
PrefabInstance PrefabSerializer::fromJson(const std::string&){ return {}; }
}
