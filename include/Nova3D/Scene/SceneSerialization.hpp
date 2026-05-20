#pragma once
#include <Nova3D/Scene/Scene.hpp>
#include <Nova3D/Serialization/JsonSerialization.hpp>

namespace nova3d::scene {
class SceneSerializer { public: static std::string saveToJson(const SceneManager& scene); };
class SceneLoader { public: static void loadFromJson(SceneManager& scene, const std::string& json); };
}
