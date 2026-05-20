#pragma once
#include <Nova3D/Core/Math/Math.hpp>
#include <Nova3D/Serialization/JsonSerialization.hpp>
#include <unordered_map>

namespace nova3d::scene {

struct Prefab { std::string type; std::string sourceSceneJson; };
struct PrefabInstance {
    Prefab prefab;
    math::Transform transformOverride{};
    std::unordered_map<std::string, std::string> propertyOverrides;
    std::unordered_map<std::string, std::string> materialOverrides;
};
class PrefabSerializer { public: static std::string toJson(const PrefabInstance& instance); static PrefabInstance fromJson(const std::string& json); };

}
