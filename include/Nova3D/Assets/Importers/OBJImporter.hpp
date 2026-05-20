#pragma once
#include <Nova3D/Assets/Importers/MeshImporter.hpp>
namespace nova3d::assets { class OBJImporter final : public MeshImporter { public: bool canImport(const std::string& extension) const override; std::shared_ptr<ModelAsset> importFromFile(const std::string& path) const override; }; }
