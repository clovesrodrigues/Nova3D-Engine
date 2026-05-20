#include <Nova3D/Scene/SceneSerialization.hpp>
namespace nova3d::scene {
std::string SceneSerializer::saveToJson(const SceneManager& scene){ serialization::JsonSerializer s; auto& r=s.root(); r["nodeCount"].value=static_cast<std::int64_t>(scene.root()->children().size()); return s.toJsonString(); }
void SceneLoader::loadFromJson(SceneManager&, const std::string&){}
}
