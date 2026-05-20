#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <Nova3D/Scene/MeshData.hpp>
namespace nova3d::assets {
struct MaterialTemplate { std::string name; std::string diffuseTexture; math::Vector3 diffuseColor{1,1,1}; };
struct ModelAsset { scene::MeshCPUData mesh; std::unordered_map<std::string, MaterialTemplate> materials; };
class MeshImporter { public: virtual ~MeshImporter()=default; virtual bool canImport(const std::string& extension) const =0; virtual std::shared_ptr<ModelAsset> importFromFile(const std::string& path) const =0; };
}
